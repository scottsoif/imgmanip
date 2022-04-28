#include <iostream>
// #include <algorithm>
// #include <armadillo>
#include <vector>
#include <string>
#include <string_view>

#include "./imgmanip/imgio/imgio.h"
#include "./imgmanip/homography.h"
#include "./imgmanip/mosaic.h"
#include "./imgmanip/convolution.h"
#include "./imgmanip/grayscale.h"
// #include "./imgmanip/imgio/pch.h"


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

  Cube<int> ratioCroppedImg = maxCrop<int>(targetImg, 50/120.0);
  write_img(ratioCroppedImg, "imgs/tiles/ratio_cropped_IMG_9738.png");

  cout << "saved imgs to directory imgs/tiles/\n" << endl;

  vector<float> avgImgRGB = getAvgColor<int>(targetImg);

  cout << "Average Colors \nR:\t" << avgImgRGB[0] << "\nG:\t" << avgImgRGB[1] <<"\nB:\t" << avgImgRGB[2] << endl;
  avgImgRGB = getAvgColor(srcImgBroccoli);
  cout << "\nBroccoli Img Average Colors \nR:\t" << avgImgRGB[0] << "\nG:\t" << avgImgRGB[1] <<"\nB:\t" << avgImgRGB[2] << endl;

 //Since we do not need to resize img up so I delete that test
  Cube<int> resizedImgDown = resize_image<int>(targetImg, 50, 50);
  write_img(resizedImgDown,  "imgs/tiles/resized_down_IMG_9738.png");



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

void test_create_mosaic() {
  string tgt_img_path = "imgs/tgt_imgs/joy2.jpeg";
  string src_img_dir = "imgs/src_imgs";

  int tile_cnt_h = 3, tile_cnt_w = 3;

  cout << "Enter the number of tiles you want in column: ";
  cin >> tile_cnt_h;
  cout << "Enter the number of tiles you want in row: ";
  cin >> tile_cnt_w;

  Cube<int> mosaic_img = create_mosaic<int>(tgt_img_path, src_img_dir, tile_cnt_h, tile_cnt_w);
  write_img(mosaic_img, "imgs/mosaic_imgs/joy2.jpg");
}

void test_convolution() {
  // string srcImgPath = "imgs/EC_plaza.png";
  string srcImgPath = "imgs/tgt_imgs/goat.jpeg";
  // string srcImgPath = "imgs/grayscale_imgs/grayscale_goat.jpg";
  // laplacian filter
  mat kernel = { { 0, -1, 0 },
                 { -1, 4, -1 },
                 { 0, -1, 0 } };

    // box filter
    // kernel = {{ 0, 0, 0, 0, 0, 0, 0, 0 } ,
    //           { 0, 0, 0, 0, 0, 0, 0, 0 } ,
    //           { 0, 0, 1, 1, 1, 1, 0, 0 },
    //           { 0, 0, 1, 1, 1, 1, 0, 0 } ,
    //           { 0, 0, 1, 1, 1, 1, 0, 0 } ,
    //           { 0, 0, 1, 1, 1, 1, 0, 0 } ,
    //           { 0, 0, 0, 0, 0, 0, 0, 0 } ,
    //           { 0, 0, 0, 0, 0, 0, 0, 0 }  };
  // kernel = {{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 } ,
  //         { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 } ,
  //         { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 } ,
  //         { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 } ,
  //         { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 } ,
  //         { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
  //         { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 } ,
  //         { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 } ,
  //         { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 } ,
  //         { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 } ,
  //         { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 } ,
  //         { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 } ,
  //         { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }  };
    // kernel /= 169; 
  // kernel = { { -1, 1 }};
  Cube<int> convolvedImg = convolve2d<int>(srcImgPath, kernel, 1);
  
  write_img(convolvedImg, "imgs/conv_results/convolvedImg.png");
}

void test_grayscale(){
  string srcImgPath = "imgs/tiles/hp_netflix.png";
  Cube<int> newImgLuma = getGrayScaledImg<int>(srcImgPath);
  write_img(newImgLuma, "imgs/grayHpLuma.png");
  Cube<int> newImgAvg = getGrayScaledImg<int>(srcImgPath,4);
  write_img(newImgAvg, "imgs/grayHpCustom.png");




}
class CmdLineArgException: public exception
{} cmdLineArgException;

void parseArgs(int argc, char const *argv[]){
  cout << "num args = " << argc-1 << endl;

  if(argc < 2){
    cout << "\nUsage options:" << endl;
    cout << "\t" << argv[0] << " —-mosaic 'tgtImage.png' ‘srcDirectory'" << endl;
    cout << "\t" << argv[0] << " ——homography 'srcImage.png' ['trapezoid' | 'spiral' | 'rTrapezoid' | 'random']" << endl;
    cout << "\t" << argv[0] << " --grayscale 'srcImage.png' \n\t\t\toptional: --shades intNumber" << endl;
    cout << "\n\t For custom usage, modify main function and choose from our wide range of tools\n" << endl;
    return;
  }


  if( string_view(argv[1])=="--mosaic" ) {
    if(argc<4){
      cerr << " Expected 3 arguments but only " << argc-1 << " were given\n" << endl;
      throw cmdLineArgException;
    }
    createMosaicCommandLine(string(argv[2]), string(argv[3]));
      
  }
  else if ( string_view(argv[1])=="--homography" ) {
    if(argc<4){
      cerr << " Expected 3 arguments but only " << argc-1 << " were given\n" << endl;
      throw cmdLineArgException;
    }
    homographyCommandLine(string(argv[2]), string(argv[3]));
      
  }
  else if ( string_view(argv[1])=="--grayscale" ) {
    if(argc!=3 && argc!=5 ){
      cerr << " Expected [2 | 4] arguments but " << argc-1 << " were given\n" << endl;
      throw cmdLineArgException;
    }
    if(argc==5){
      if(string_view(argv[3]).substr(0)!="--shades"){
        cerr << "Invalid arg\n" << endl;
        throw cmdLineArgException;
      }     
      grayscaleCommandLine(string(argv[2]), string(argv[4]));
    } else {
      grayscaleCommandLine(string(argv[2]), "");
    }
  }


  // string_view(argv[1]).substr(2);
}



int main(int argc, char const *argv[])
{
  /* code */
  // Cube<int> img = read_img<int>("test.jpg");
  // testHomography(img);
  // testMosiac();
  // test_create_mosaic();
  // test_convolution();
  //test_grayscale();
  parseArgs(argc, argv);

  return 0;
}

