#ifndef HOMOGRAPHY_H_
#define HOMOGRAPHY_H_

#include <iostream>
// #include <algorithm>
// #include <armadillo>
#include <vector>
#include "imgio/imgio.h"

using namespace std;
using namespace arma;


/**
 * @brief Given source points and destination points, compute homography matrix
 *
 * @tparam rowColType the pixel type
 * @param srcPts the source points
 * @param destPts the destination points
 * @return mat the homography matrix
 */
template <typename rowColType>
 mat computeHomography (Mat<rowColType>& srcPts, Mat<rowColType>& destPts){
   int n_row = srcPts.n_rows;
   int n_col = 9;
   Mat<rowColType> newSrcPts(2*n_row, n_col, fill::zeros);
   for (int i = 0; i< n_row; i++){
     rowColType x_s = srcPts.at(i, 0);
     rowColType y_s = srcPts.at(i, 1);
     rowColType x_d = destPts.at(i,0);
     rowColType y_d = destPts.at(i,1);
     Mat<rowColType> temp = {{x_s,y_s,1,0,0,0, -x_d*x_s, -x_d*y_s, -x_d},
                              {0,0,0,x_s,y_s, 1, -y_d*x_s, -y_d*y_s, -y_d}};
     newSrcPts.rows(2*i, 2*i+1) = temp;

   }

  vec D;// eigen value diagonal no need to diagnolize
  mat V;
  mat A = conv_to<mat>::from(newSrcPts);
  mat finalA = (A.t() )*A;

  eig_sym(D,V,finalA);
  //double minD = min(diagvec(D));
  int minDIdx = index_min(D);
  mat H_3x3 = V.col(minDIdx);
  H_3x3.reshape(3,3);
  H_3x3 = H_3x3.t();

  return H_3x3;

 }

/**
 * @brief Apply the homography matrix
 *
 * @tparam rowColType the pixel type
 * @param H_3x3 the homography matrix
 * @param srcPtsSqueezed the source points
 * @return Mat<int> the computed desintation
 */
template <typename rowColType>
Mat<int> applyHomography(mat H_3x3, Mat<rowColType> srcPtsSqueezed){

  Mat<rowColType> expandOnes(srcPtsSqueezed.n_rows, 1,fill::ones);
  Mat<rowColType> srcPts = join_rows(srcPtsSqueezed, expandOnes);

  mat destPtsTilda =  H_3x3 * srcPts.t();

  mat destPts(2, srcPtsSqueezed.n_rows, fill::zeros);
  // doing it this way because broadcast division wasn't
  // working well in armadillo for 2 rows
  destPts(0, span::all) =  destPtsTilda(0, span::all) / destPtsTilda(2, span::all);
  destPts(1, span::all) =  destPtsTilda(1, span::all) / destPtsTilda(2, span::all);

  destPts =  round(destPts);
  Mat<int> destPts_int = conv_to<Mat<int>>::from(destPts);
  return destPts_int.t();

}
/**
 * @brief Get the New Canvas Dimentions
 *
 * @param srcImg the souce image
 * @param H_3x3 the homography matrix
 * @return vector<int> the number of rows and number of cols of the new image
 */
vector<int> getNewCanvasDims(Cube<int>& srcImg, mat& H_3x3){

  double right_idx = (double)(srcImg.n_cols-1);
  double bottom_idx = (double)(srcImg.n_rows-1);


  mat corners =  { { 0, 0 },                  // top-left
                  { 0, right_idx },          // top-right
                  { bottom_idx, right_idx }, // bottom-right
                  { bottom_idx, 0 } };       // bottom-left

  Mat<int> newCorners = applyHomography(H_3x3, corners);

  int colMax = newCorners(span::all, 1).max();
  int colMin = newCorners(span::all, 1).min();
  int rowMax = newCorners(span::all, 0).max();
  int rowMin = newCorners(span::all, 0).min();
  int nCols =colMax-colMin;
  int nRows =rowMax-rowMin;

  return {nRows+1, nCols+1, abs(colMin), abs(rowMin)};

}
/**
 * @brief get the warped image
 *
 * @tparam pixel_type the type of pixel
 * @param srcImg the souce image
 * @param H_3x3 the homography matrix
 * @return Cube<pixel_type> the warped image
 */
template <NumericType pixel_type>
Cube<pixel_type> genHomographyImgCanvas(Cube<int>& srcImg, mat& H_3x3){

  vector<int> canvasDims =  getNewCanvasDims(srcImg, H_3x3);
  int colOffset=canvasDims[2], rowOffset=canvasDims[3];
  Cube<pixel_type> newImg(canvasDims[0], canvasDims[1], 3);
  newImg.fill(0);

  mat invH_3x3 = inv(H_3x3);

  int srcN_cols = (int)(srcImg.n_cols-1);
  int srcN_rows = (int)(srcImg.n_rows-1);
  Mat<int> srcPt = {{0,0}};
  Mat<int> destPt = {{0,0}};
  // int =
  for(int i=0; i<canvasDims[0]; i++){
    for(int j=0; j<canvasDims[1]; j++){
      srcPt =  { { i-rowOffset,  j-colOffset} };
      destPt =  applyHomography(invH_3x3, srcPt);

      if (destPt[0] > 0 && destPt[0] <= srcN_rows && destPt[1] > 0 && destPt[1] <= srcN_cols){
          newImg(i, j , 0) = srcImg(destPt[0], destPt[1], 0);
          newImg(i, j , 1) = srcImg(destPt[0], destPt[1], 1);
          newImg(i, j , 2) = srcImg(destPt[0], destPt[1], 2);
          // cout << "yay" << endl;
      }
      // cout << "img test2 \n" << +srcImg(span(0,0), span(0,0), span::all) << endl;

    }
  }
  // Cube<pixel_type> newImg(5, 5, 3);


  return newImg;

}
/**
 * @brief The command line for the homography
 *
 * @param srcImgPath the path of source image
 * @param homogType the type of homography -- triangle, spiral, random
 */
void homographyCommandLine(string srcImgPath, string homogType) {


  Cube<int> srcImg = read_img<int>(srcImgPath);
  mat H_3x3 = {{0, 0.1}};

  double right_idx = (double)(srcImg.n_cols-1);
  double bottom_idx = (double)(srcImg.n_rows-1);
  mat startPoints =  { { 0,  0}, // top-left
                      {0, right_idx}, //top- right
                      { bottom_idx, right_idx }, //bottom-right
                      {bottom_idx, 0}}; // bottom-left

  if(homogType=="spiral"){
    double shearFactor = .3;
    double shearDim = shearFactor*(right_idx+bottom_idx)/2;
    mat destination = {{0,shearDim}, // top-left
                      {shearDim, right_idx+0}, // top-right
                      { bottom_idx+0,  right_idx-shearDim }, // bot-right
                      {bottom_idx-shearDim, 0+0}}; // bot-left
    H_3x3 = computeHomography(startPoints, destination);

  }
  else if (homogType=="triangle"){
    mat destination = {{0,right_idx/2-right_idx/7}, // top-left
                  {0, right_idx/2+right_idx/7}, // top-right
                  { bottom_idx,  right_idx }, // bot-right
                  {bottom_idx, 0+0}}; // bot-left
    H_3x3 = computeHomography(startPoints, destination);
  }
  else if (homogType=="rTrapezoid"){
    mat destination = {{0,right_idx/3}, // top-left
                  {0, right_idx}, // top-right
                  { bottom_idx,  right_idx }, // top-left
                  {bottom_idx, 0+0}}; // top-right
    H_3x3 = computeHomography(startPoints, destination);
  }
  else if (homogType=="class_demo"){

    // original points
    // 1483, 1332 // top left
    // 1696,2144 // top right
    // 2833, 2156 // bot right
    // 3086,1336 // bot left
    mat destination = {{0, 0},
                  {1696-1483, 2144-1332},
                  { 2834-1483, 2168-1332 },
                  {3086-1483, 1329-1336}};
    H_3x3 = computeHomography(startPoints, destination);
  }
  else {
        // random test H_matrix
    homogType = "random";
    H_3x3 = { { -0.0043,  0.0004, -0.4261 },
              {  0.0020, -0.0054, -0.9046 },
              {  0.0000,  0.0000, -0.0091 } };
  }
  Cube<int> newImg = genHomographyImgCanvas<int>(srcImg, H_3x3);

  string outFileName = "imgs/homog_imgs/homog_" + homogType + "_";
  int srcNameIdx = srcImgPath.find_last_of("/")+1;
  outFileName += srcImgPath.substr(srcNameIdx);

  write_img(newImg, outFileName);

  }

#endif
