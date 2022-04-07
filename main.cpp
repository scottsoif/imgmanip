#include <iostream>
// #include <algorithm>
#include <armadillo>
#include <vector>
#include "imgmanip/imgio/imgio.h"

using namespace std;
using namespace arma;


int test_arm() {
    cout << "Armadillo version: " << arma_version::as_string() << endl;

  // construct a matrix according to given size and form of element initialisation
  mat A(2,3,fill::zeros);

  // .n_rows and .n_cols are read only
  cout << "A.n_rows: " << A.n_rows << endl;
  cout << "A.n_cols: " << A.n_cols << endl;

  A(1,2) = 456.0;  // access an element (indexing starts at 0)
  A.print("A:");

  A = 5.0;         // scalars are treated as a 1x1 matrix
  A.print("A:");

  A.set_size(4,5); // change the size (data is not preserved)

  A.fill(5.0);     // set all elements to a specific value
  A.print("A:");

  A = { { 0.165300, 0.454037, 0.995795, 0.124098, 0.047084 },
        { 0.688782, 0.036549, 0.552848, 0.937664, 0.866401 },
        { 0.348740, 0.479388, 0.506228, 0.145673, 0.491547 },
        { 0.148678, 0.682258, 0.571154, 0.874724, 0.444632 },
        { 0.245726, 0.595218, 0.409327, 0.367827, 0.385736 } };

  A.print("A:");

  // determinant
  cout << "det(A): " << det(A) << endl;

  // inverse
  cout << "inv(A): " << endl << inv(A) << endl;

  // save matrix as a text file
  A.save("A.txt", raw_ascii);

  // load from file
  mat B;
  B.load("A.txt");

  // submatrices
  cout << "B( span(0,2), span(3,4) ):" << endl << B( span(0,2), span(3,4) ) << endl;

  cout << "B( 0,3, size(3,2) ):" << endl << B( 0,3, size(3,2) ) << endl;

  cout << "B.row(0): " << endl << B.row(0) << endl;

  cout << "B.col(1): " << endl << B.col(1) << endl;

  // transpose
  cout << "B.t(): " << endl << B.t() << endl;

  // maximum from each column (traverse along rows)
  cout << "max(B): " << endl << max(B) << endl;

  // maximum from each row (traverse along columns)
  cout << "max(B,1): " << endl << max(B,1) << endl;

  // maximum value in B
  cout << "max(max(B)) = " << max(max(B)) << endl;

  // sum of each column (traverse along rows)
  cout << "sum(B): " << endl << sum(B) << endl;

  // sum of each row (traverse along columns)
  cout << "sum(B,1) =" << endl << sum(B,1) << endl;

  // sum of all elements
  cout << "accu(B): " << accu(B) << endl;

  // trace = sum along diagonal
  cout << "trace(B): " << trace(B) << endl;

  // generate the identity matrix
  mat C = eye<mat>(4,4);

  // random matrix with values uniformly distributed in the [0,1] interval
  mat D = randu<mat>(4,4);
  D.print("D:");

  // row vectors are treated like a matrix with one row
  rowvec r = { 0.59119, 0.77321, 0.60275, 0.35887, 0.51683 };
  r.print("r:");

  // column vectors are treated like a matrix with one column
  vec q = { 0.14333, 0.59478, 0.14481, 0.58558, 0.60809 };
  q.print("q:");

  // convert matrix to vector; data in matrices is stored column-by-column
  vec v = vectorise(A);
  v.print("v:");

  // dot or inner product
  cout << "as_scalar(r*q): " << as_scalar(r*q) << endl;

  // outer product
  cout << "q*r: " << endl << q*r << endl;

  // multiply-and-accumulate operation (no temporary matrices are created)
  cout << "accu(A % B) = " << accu(A % B) << endl;

  // example of a compound operation
  B += 2.0 * A.t();
  B.print("B:");

  // imat specifies an integer matrix
  imat AA = { { 1, 2, 3 },
              { 4, 5, 6 },
              { 7, 8, 9 } };

  imat BB = { { 3, 2, 1 },
              { 6, 5, 4 },
              { 9, 8, 7 } };

  // comparison of matrices (element-wise); output of a relational operator is a umat
  umat ZZ = (AA >= BB);
  ZZ.print("ZZ:");

  // cubes ("3D matrices")
  cube Q( B.n_rows, B.n_cols, 2 );

  Q.slice(0) = B;
  Q.slice(1) = 2.0 * B;

  Q.print("Q:");

  // 2D field of matrices; 3D fields are also supported
  field<mat> F(4,3);

  for(uword col=0; col < F.n_cols; ++col)
  for(uword row=0; row < F.n_rows; ++row)
    {
    F(row,col) = randu<mat>(2,3);  // each element in field<mat> is a matrix
    }

  F.print("F:");

  return 0;
}

// TODO fix
int tempMax(int num1, int num2){
  return (num1 > num2) ? num1 : num2;
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


void testHomography(Cube<int> srcImg){

  mat H_3x3 = { { -0.0043,  0.0004, -0.4261 },
            {  0.0020, -0.0054, -0.9046 },
            {  0.0000,  0.0000, -0.0091 } };

  // mat srcPtsSqueezed =  { { 162.0000,  102.0000 },
  //                { 640.0000,  100.0000 },
  //                { 642.0000,  698.0000 },
  //                { 162.0000,  696.0000 } };


  Cube<int> newImg = genHomographyImgCanvas<int>(srcImg, H_3x3);
  write_img(newImg, "testHomog.jpeg");


}

int main(int argc, char const *argv[])
{
  /* code */
  // test_arm();
  imgio_hello_word();
  Cube<int> img = read_img<int>("test.jpg");
  testHomography(img);
  write_img(img, "test3.jpeg");
  return 0;
}
