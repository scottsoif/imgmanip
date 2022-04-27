#ifndef GRAYSCALE_H_
#define GRAYSCALE_H_

#include <iostream>
#include "imgio/imgio.h"
#include <string>

using namespace std;
using namespace arma;
/**
 * @brief Get the Gray Scaled Img object with simple luma algorithm 
 * 
 * @tparam pixel_type the type of the pixel 
 * @param srcImgPath  the source image path
 * @return Cube<pixel_type>  the new gray scaled image
 */
template<NumericType pixel_type>
Cube<pixel_type> getGrayScaledImg(string srcImgPath){

    Cube<pixel_type> srcImg = read_img<pixel_type>(srcImgPath);

    Cube<pixel_type> newImg (srcImg.n_rows, srcImg.n_cols, srcImg.n_slices);
    float param[3] = {0.3, 0.11, 0.59};
    for(int i =0; i < srcImg.n_rows; i++){
        for(int j = 0; j< srcImg.n_cols; j++){
            int result = srcImg.at(i,j,0)*param[0]+srcImg.at(i,j,1)*param[1] + srcImg.at(i,j,2)*param[2];
            newImg(i,j,0) = result;
            newImg(i,j,1) = result;
            newImg(i,j,2) = result;
        }
    }
    return newImg;
}
/**
 * @brief Get the Gray Scale Customed Shades object
 * 
 * @tparam pixel_type the type of the pixel 
 * @param srcImgPath the source image path
 * @param numberOfShades  the number of shades you hope to get from 2 - 255
 * @return Cube<pixel_type> The gray scale image 
 */
template<NumericType pixel_type>
Cube<pixel_type> getGrayScaledImg(string srcImgPath, int numberOfShades){

    Cube<pixel_type> srcImg = read_img<pixel_type>(srcImgPath);

    Cube<pixel_type> newImg (srcImg.n_rows, srcImg.n_cols, srcImg.n_slices);
    float conversionFactor = 255/(numberOfShades - 1);
     for(int i =0; i < srcImg.n_rows; i++){
        for(int j = 0; j< srcImg.n_cols; j++){
            float avg = (srcImg.at(i,j,0)+srcImg.at(i,j,1)+srcImg.at(i,j,2))/3;
            int result = (avg/conversionFactor+0.5)*conversionFactor;
            newImg(i,j,0) = result;
            newImg(i,j,1) = result;
            newImg(i,j,2) = result;
        }
    }

    return newImg;


}

void grayscaleCommandLine(string srcImgPath, string numShadesArg) {

    Cube<int> newImgAvg;
    string outFileName = "imgs/grayscale_imgs/grayscale_";

    if(numShadesArg==""){
        newImgAvg = getGrayScaledImg<int>(srcImgPath);
    }
    else {
        try {
            int numShades = stoi(numShadesArg);
            outFileName += numShadesArg + "_shades_";
            newImgAvg = getGrayScaledImg<int>(srcImgPath,numShades);

        }
        catch (const std::invalid_argument& ia) {
            std::cerr << "\nInvalid argument: " << ia.what() << '\n';
        }
    }

    int srcNameIdx = srcImgPath.find_last_of("/")+1;
    outFileName += srcImgPath.substr(srcNameIdx);

    write_img(newImgAvg, outFileName);

}


#endif