#include <iostream>
// #include <algorithm>
#include <armadillo>
#include <vector>
#include "./imgmanip/imgio/imgio.h"
#include "./imgmanip/homography.h"


using namespace std;
using namespace arma;



void testHomography(Cube<int> srcImg){

  // random test H_matrix
  // mat H_3x3 = { { -0.0043,  0.0004, -0.4261 },
  //           {  0.0020, -0.0054, -0.9046 },
  //           {  0.0000,  0.0000, -0.0091 } };
  double right_idx = (double)(srcImg.n_cols-1);
  double bottom_idx = (double)(srcImg.n_rows-1);
  mat startPoints =  { { 0,  0}, // top-left
                       {0, right_idx}, //top- right
                       { bottom_idx, right_idx }, //bottom-right
                       {bottom_idx, 0}}; // bottom-left
  mat destination = {{50,5}, 
  {0, right_idx-50},
  { bottom_idx,  right_idx },
  {bottom_idx, 0}};

  mat H_3x3 = computeHomography(startPoints, destination);
  Cube<int> newImg = genHomographyImgCanvas<int>(srcImg, H_3x3);
  
  write_img(newImg, "testHomog.jpeg");


}

int main(int argc, char const *argv[])
{
  /* code */
  imgio_hello_word();
  Cube<int> img = read_img<int>("test.jpg");


  
  //cout << img(0,0,0) <<endl;
  testHomography(img);
  write_img(img, "test3.jpeg");
  return 0;
}
