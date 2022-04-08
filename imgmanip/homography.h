// module;
#include <iostream>
// #include <algorithm>
#include <armadillo>
#include <vector>
#include "imgio/imgio.h"

using namespace std;
using namespace arma;


// TODO fix
int tempMax(int num1, int num2){
  return (num1 > num2) ? num1 : num2;
}



template <typename rowColType>
 mat computeHomography (Mat<rowColType> srcPts, Mat<rowColType> destPts){
   int n_row = srcPts.n_rows;
   int n_col = 9;
   Mat<rowColType> newSrcPts(n_row, n_col, fill::zeros);
   for (int i = 0; i< n_row; i++){
     rowColType x_s = srcPts.at(i,1);
     rowColType y_s = srcPts.at(i,2);
     rowColType x_d = destPts.at(i,1);
     rowColType y_d = destPts.at(i,2);
     Mat<rowColType> temp = {{x_s,y_s,1,0,0,0, -x_d*x_s, -x_d*y_s, -x_d},
                              {0,0,0,x_s,y_s, 1, -y_d*x_s, -y_d*y_s, -y_d}};
     newSrcPts.rows(2*i-1, 2*1) = temp;


   }
   mat V;
   mat D;
   eig_gen(V,D,newSrcPts.t()*newSrcPts);
   //double minD = min(diagvec(D));
   int minDIdx = index_min(diagvec(D));
   mat H_3x3 = V.col(minDIdx);
   H_3x3.reshape(3,3);
   return H_3x3.t();




 }
// template <typename pixel_type>
// Cube<pixel_type> applyHomography(mat H_3x3, mat srcPts){
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

vector<int> getNewCanvasDims(Cube<int>& srcImg, mat& H_3x3){

  double right_idx = (double)(srcImg.n_cols-1);
  double bottom_idx = (double)(srcImg.n_rows-1);
  mat rightCorners =  { { 0,  right_idx }, // top-right
                        { bottom_idx,  right_idx } }; // bottom-right

  Mat<int> newRightCorners = applyHomography(H_3x3, rightCorners);

  int nCols = newRightCorners(span::all, 1).max();
  int nRows = tempMax((int)bottom_idx, newRightCorners(1,0) );

  return {nRows+1, nCols+1};

}

template <typename pixel_type>
Cube<pixel_type> genHomographyImgCanvas(Cube<int>& srcImg, mat& H_3x3){

  vector<int> canvasDims =  getNewCanvasDims(srcImg, H_3x3);
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
      srcPt =  { { i,  j } };
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

