// module;
#include <iostream>
// #include <algorithm>
#include <armadillo>
#include <vector>
#include "imgio/imgio.h"
#include <chrono>


using namespace std;
using namespace arma;

typedef std::chrono::duration<double, std::milli> millisec_type;


template <typename pixel_type>
Cube<pixel_type> crop(Cube<int>& srcImg, int left_top_r, int left_top_c, int w, int h){

    Cube<pixel_type> croppedImg(h, w, 3);
    croppedImg.fill(0);

    int srcN_rows = (int)(srcImg.n_rows);
    int srcN_cols = (int)(srcImg.n_cols);

    int rwStartIdx = max(0, left_top_r);
    int rwEndIdx = min(srcN_rows-1, rwStartIdx+h-1);
    int clStartIdx = max(0, left_top_c);
    int clEndIdx = min(srcN_cols-1, clStartIdx+w-1);

    croppedImg.subcube(0, 0, 0, (rwEndIdx-rwStartIdx), (clEndIdx-clStartIdx), 2)  = 
                srcImg.subcube(rwStartIdx, clStartIdx, 0, rwEndIdx, clEndIdx,2);

    return croppedImg;
}


template <typename pixel_type>
Cube<pixel_type> max_crop(Cube<int>& srcImg, double img_out_ratio){
    // function to center crop img to specified aspect ratio

    double srcN_h = (double)(srcImg.n_rows);
    double srcN_w = (double)(srcImg.n_cols);

    double currRatio = srcN_w/srcN_h;

    // new dims before adjusting for ratio
    // either newH or newW will be modified later
    int newH = srcN_h;
    int newW = srcN_w;
    int rwStartIdx = 0;
    int clStartIdx = 0;

    if(currRatio < img_out_ratio){
        newH = (int)(srcN_w/img_out_ratio);
        rwStartIdx = (srcN_h-newH)/2; // align cropped img in center
    }
    else {
        newW = (int)(srcN_h*img_out_ratio);
        clStartIdx = (srcN_w-newW)/2; // align cropped img in center
    }


    Cube<pixel_type> ratioCroppedImg(newH, newW, 3);
    ratioCroppedImg.fill(0);

    // end idx for span
    int rwEndIdx = rwStartIdx+newH-1; 
    int clEndIdx = clStartIdx+newW-1;


    ratioCroppedImg.subcube(0, 0, 0, newH-1, newW-1, 2)  = 
                srcImg.subcube(rwStartIdx, clStartIdx, 0, rwEndIdx, clEndIdx,2);

    return ratioCroppedImg;
}

vector<float> getAvgColor(Cube<int>& img){

    int numElem = (int)img.n_rows*img.n_cols;

    int sumR = accu(img.slice(0));
    int sumG = accu(img.slice(1));
    int sumB = accu(img.slice(2));

    float avgR = (float)sumR/numElem;
    float avgG = (float)sumG/numElem;
    float avgB = (float)sumB/numElem;

    return {avgR, avgG, avgB};
} 



