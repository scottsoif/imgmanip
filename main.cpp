#include <iostream>
// #include <algorithm>
#include <armadillo>
#include <vector>
#include "./imgmanip/imgio/imgio.h"
#include "./imgmanip/homography.h"
#include "./imgmanip/mosaic.h"


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
  mat destination = {{0,50}, 
  {50, right_idx+0},
  { bottom_idx+0,  right_idx-50 },
  {bottom_idx-50, 0+0}};

  mat H_3x3 = computeHomography(startPoints, destination);
  Cube<int> newImg = genHomographyImgCanvas<int>(srcImg, H_3x3);
  
  write_img(newImg, "testHomog.jpeg");


}

void testMosiac(){

  
  Cube<int> srcImgBroccoli = read_img<int>("imgs/tiles/broccoli.png");
  Cube<int> srcHp = read_img<int>("imgs/tiles/hp_netflix.png");
  Cube<int> tgtLettuce = read_img<int>("imgs/tiles/lettuce.png");
  Cube<int> targetImg = read_img<int>("imgs/tiles/IMG_9738.png");

  vector<Cube<int>> srcImgList;
  srcImgList.push_back(srcImgBroccoli);
  srcImgList.push_back(srcHp);

  // demonstrating that the h or w can be greater than img size
  Cube<int> croppedImg = crop<int>(targetImg, 1800, 600, 7000, 400);
  write_img(croppedImg, "imgs/tiles/simple_cropped_IMG_9738.png");

  Cube<int> ratioCroppedImg = max_crop<int>(targetImg, 50/120.0);
  write_img(ratioCroppedImg, "imgs/tiles/ratio_cropped_IMG_9738.png");
  
  cout << "saved imgs to directory imgs/tiles/\n" << endl;

  vector<float> avgImgRGB = getAvgColor<int>(targetImg);

  cout << "Average Colors \nR:\t" << avgImgRGB[0] << "\nG:\t" << avgImgRGB[1] <<"\nB:\t" << avgImgRGB[2] << endl;
  avgImgRGB = getAvgColor(srcImgBroccoli);
  cout << "\nBroccoli Img Average Colors \nR:\t" << avgImgRGB[0] << "\nG:\t" << avgImgRGB[1] <<"\nB:\t" << avgImgRGB[2] << endl;
  
 //Since we do not need to resize img up so I delete that test
  Cube<int> resizedImgDown = resize_image<int>(targetImg, 50, 50); 
  write_img(resizedImgDown, "imgs/tiles/resized_down_IMG_9738.png");
  



  //should be the hp one 
  Cube<int> bestMatchImg = getBestMatch<int>(targetImg, srcImgList);
  write_img (bestMatchImg,"imgs/tiles/bestMatchTargetNight.png");
  //should be the lettuce
  Cube<int> bestMatchImg2 = getBestMatch<int>(tgtLettuce, srcImgList);
  write_img (bestMatchImg2,"imgs/tiles/bestMatchTargetLettuce.png");

  tuple<Cube<int>, vector<float>> tileAndAvgColor = init_tile<int>(srcHp, 0.8, 1000, 800);
  Cube<int> tileHp = std::get<0>(tileAndAvgColor);
  vector<float> avgHp = std::get<1>(tileAndAvgColor);
  write_img (tileHp,"imgs/tiles/tileHp.png");
  cout << "tile Hp avg Color is \nR:\t" << avgHp[0]  << "\nG:\t " << avgHp[1] << "\nB:\t" << avgHp[2] << endl;





  





}


int main(int argc, char const *argv[])
{
  /* code */
  // imgio_hello_word();
  Cube<int> img = read_img<int>("test.jpg");
  testHomography(img);

  testMosiac();

  write_img(img, "test3.jpeg");
  return 0;
}
