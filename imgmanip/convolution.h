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

typedef std::chrono::duration<double, std::milli> millisec_type;


/**
 * @brief create a mosaic version of the image on tgt_img_path from a list of images in the
 *        src_img_dir. The mosaic version of image has tile_cnt_h x tile_cnt_w mosaics
 *
 * @tparam pixel_type The type of the pixel
 * @param tgt_img_path the path to the target image
 * @param src_img_dir the directory containing potential mosaics
 * @param tile_cnt_h the number of tiles in a column of the resulting image
 * @param tile_cnt_w the number of tiles in a row of the resulting image
 */

template<NumericType pixel_type>
vector<int> getConvolvedImgSize(Cube<pixel_type>& srcImg, mat& kernel, int stride){

    int newH = (srcImg.n_rows-kernel.n_rows)/stride + 1;
    int newW = (srcImg.n_cols-kernel.n_cols)/stride + 1;

    return {newH, newW};
}

mat flipKernel(mat& kernel){
    kernel = fliplr(kernel);
    kernel = flipud(kernel);
    return kernel;
}


template<NumericType pixel_type>
Cube<pixel_type> convolve2d(string srcImgPath, mat& kernel, int stride){

    Cube<pixel_type> srcImg = read_img<pixel_type>(srcImgPath);
    Mat<pixel_type> srcImgSlice = srcImg.slice(0);
    
    vector<int> newImgSize =  getConvolvedImgSize(srcImg, kernel, stride);
    Cube<pixel_type> convolvedImg(newImgSize[0], newImgSize[1], 3);
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