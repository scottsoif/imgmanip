// module;
#include <iostream>
// #include <algorithm>
#include <armadillo>
#include <vector>
#include "imgio/imgio.h"
#include <chrono>


using namespace std;
using namespace arma;

// crop(image, left_bottom_r, left_bottom_c, width, height) -> image
// max_crop(image, img_out_ratio: w/h) -> image
// get_avg_color(image) -> int[3]

typedef std::chrono::duration<double, std::milli> millisec_type;


template <typename pixel_type>
Cube<pixel_type> crop(Cube<int>& srcImg, int left_top_r, int left_top_c, int w, int h){

    Cube<pixel_type> croppedImg(h, w, 3);
    croppedImg.fill(0);

    int srcN_rows = (int)(srcImg.n_rows);
    int srcN_cols = (int)(srcImg.n_cols);

    int rwStartIdx = max(0, left_top_r);
    int rwEndIdx = min(srcN_rows-1, h-1);
    int clStartIdx = max(0, left_top_c);
    int clEndIdx = min(srcN_cols-1, w-1);

    auto start1 = chrono::steady_clock::now();
    auto start2 = chrono::steady_clock::now();
    auto end1 = chrono::steady_clock::now();
    auto end2 = chrono::steady_clock::now();
    millisec_type t1(end1-start1);
    millisec_type t2(end1-start1);

    for(int k=0; k<3; k++){
        cout << "Trial # " << k << endl;

        croppedImg.fill(0);

        start2 = chrono::steady_clock::now();
        croppedImg.subcube(rwStartIdx, clStartIdx, 0, rwEndIdx, clEndIdx, 2)  = 
                    srcImg.subcube(rwStartIdx, clStartIdx, 0, rwEndIdx, clEndIdx,2);
        end2 = chrono::steady_clock::now();
        millisec_type t2(end2-start2);
        cout <<  "Testing slicing time: "  << t2.count() << "\n";


        croppedImg.fill(0);

        start1 = chrono::steady_clock::now();
        for(int i=0; i<h && i<srcN_rows; i++){
            for(int j=0; j<w && j<srcN_cols; j++){

                croppedImg(i, j , 0) = srcImg(i, j, 0);
                croppedImg(i, j , 1) = srcImg(i, j, 1);
                croppedImg(i, j , 2) = srcImg(i, j, 2);
    
                } 
            }
        end1 = chrono::steady_clock::now();
        millisec_type t1(end1-start1);
        cout <<  "Testing for loop time: "  << t1.count() << "\n*****\n\n";

    }
    return croppedImg;
}



