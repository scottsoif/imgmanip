#ifndef CONVOLUTION_H_
#define CONVOLUTION_H_

#include <iostream>
#include <vector>
#include <future>
#include "imgio/imgio.h"
#include <chrono>
#include <string>

using namespace std;
using namespace arma;


/**
 * @brief Get the size of the convolved img 
 * 
 * @tparam pixel_type the type of the pixel
 * @param srcImg the source img
 * @param kernel the kernel for convolution
 * @param stride the stride for convolution
 * @return vector<int> the vector of the height and width of new img 
 */

template<NumericType pixel_type>
vector<int> getConvolvedImgSize(Cube<pixel_type>& srcImg, mat& kernel, int stride){

    int newH = (srcImg.n_rows-kernel.n_rows)/stride + 1;
    int newW = (srcImg.n_cols-kernel.n_cols)/stride + 1;

    return {newH, newW};
}
/**
 * @brief flip the kernel
 * 
 * @param kernel the kernel for convolution
 * @return mat the flipped kernel
 */
mat flipKernel(mat& kernel){
    kernel = fliplr(kernel);
    kernel = flipud(kernel);
    return kernel;
}

/**
 * @brief Create the new convolved img
 * 
 * @tparam pixel_type the type of the pixel
 * @param srcImgPath the souce image path
 * @param kernel the kernel used for convolution
 * @param stride the stride used for convolution
 * @return Cube<pixel_type> the convolved image
 */
template<NumericType pixel_type>
Cube<pixel_type> convolve2d(string srcImgPath, mat& kernel, int stride){

    Cube<pixel_type> srcImg = read_img<pixel_type>(srcImgPath);
    Mat<pixel_type> srcImgSlice = srcImg.slice(0);
    
    vector<int> newImgSize =  getConvolvedImgSize(srcImg, kernel, stride);
    Cube<pixel_type> convolvedImg(newImgSize[0], newImgSize[1], (int)srcImg.n_slices);
    convolvedImg.fill(0);

    int rowBoundary = srcImg.n_rows-kernel.n_rows+1;
    int colBoundary = srcImg.n_cols-kernel.n_cols+1;
    int y2 = 0, x2 = 0, i = 0, j = 0;
    int convValue = 0;

    for(int slice_i = 0; slice_i< (int)srcImg.n_slices; slice_i++){
        srcImgSlice = srcImg.slice(slice_i);

        for(int y = 0; y < rowBoundary; y+=stride){
            for(int x = 0; x < colBoundary; x+=stride){

                y2 = y+kernel.n_rows-1;
                x2 = x+kernel.n_cols-1;
                // cout << "row,col " << y2 << "  " << x2 << " y,x " << y << ", " << x << endl;
                convValue = dot(srcImgSlice.submat(y, x, y2,x2), kernel);

                convolvedImg(i, j, slice_i) = convValue;
                j++;
            }
            i++;
            j = 0;
        }
        i = 0;
    }

    return convolvedImg;
}



    // croppedImg.subcube(0, 0, 0, (rwEndIdx - rwStartIdx), (clEndIdx - clStartIdx), 2) =
    //     srcImg.subcube(rwStartIdx, clStartIdx, 0, rwEndIdx, clEndIdx, 2);


#endif