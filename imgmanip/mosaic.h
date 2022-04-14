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
Cube<pixel_type> crop(Cube<pixel_type> &srcImg, int left_top_r, int left_top_c, int w, int h)
{

    Cube<pixel_type> croppedImg(h, w, 3);
    croppedImg.fill(0);

    int srcN_rows = (int)(srcImg.n_rows);
    int srcN_cols = (int)(srcImg.n_cols);

    int rwStartIdx = max(0, left_top_r);
    int rwEndIdx = min(srcN_rows - 1, rwStartIdx + h - 1);
    int clStartIdx = max(0, left_top_c);
    int clEndIdx = min(srcN_cols - 1, clStartIdx + w - 1);

    croppedImg.subcube(0, 0, 0, (rwEndIdx - rwStartIdx), (clEndIdx - clStartIdx), 2) =
        srcImg.subcube(rwStartIdx, clStartIdx, 0, rwEndIdx, clEndIdx, 2);

    return croppedImg;
}

template <typename pixel_type>
Cube<pixel_type> maxCrop(Cube<pixel_type> &srcImg, double img_out_ratio)
{
    // function to center crop img to specified aspect ratio

    double srcN_h = (double)(srcImg.n_rows);
    double srcN_w = (double)(srcImg.n_cols);

    double currRatio = srcN_w / srcN_h;

    // new dims before adjusting for ratio
    // either newH or newW will be modified later
    int newH = srcN_h;
    int newW = srcN_w;
    int rwStartIdx = 0;
    int clStartIdx = 0;

    if (currRatio < img_out_ratio)
    {
        newH = (int)(srcN_w / img_out_ratio);
        rwStartIdx = (srcN_h - newH) / 2; // align cropped img in center
    }
    else
    {
        newW = (int)(srcN_h * img_out_ratio);
        clStartIdx = (srcN_w - newW) / 2; // align cropped img in center
    }

    Cube<pixel_type> ratioCroppedImg(newH, newW, 3);
    ratioCroppedImg.fill(0);

    // end idx for span
    int rwEndIdx = rwStartIdx + newH - 1;
    int clEndIdx = clStartIdx + newW - 1;

    ratioCroppedImg.subcube(0, 0, 0, newH - 1, newW - 1, 2) =
        srcImg.subcube(rwStartIdx, clStartIdx, 0, rwEndIdx, clEndIdx, 2);

    return ratioCroppedImg;
}
template<typename pixel_type>
vector<float> getAvgColor(Cube<pixel_type> &img)
{

    int numElem = (int)img.n_rows * img.n_cols;

    int sumR = accu(img.slice(0));
    int sumG = accu(img.slice(1));
    int sumB = accu(img.slice(2));

    float avgR = (float)sumR / numElem;
    float avgG = (float)sumG / numElem;
    float avgB = (float)sumB / numElem;

    return {avgR, avgG, avgB};
}

/**
 * @brief This function resize the image, it can be scaled up and scaled down
 * 
 * @tparam pixel_type The type of the pixel
 * @param srcImg The source img
 * @param target_h  The height of the target resized img
 * @param target_w  The width of the target resized img
 * @return Cube<pixel_type> 
 */
template <typename pixel_type>
Cube<pixel_type> resize_image(Cube<pixel_type> &srcImg, int target_h, int target_w)
{
    int srcN_h = srcImg.n_rows;
    int srcN_w = srcImg.n_cols;
   //calculate the scaling factor
    double scale_x = ((double)(target_h)) / ((double)(srcN_h));
    double scale_y = ((double)(target_w)) / ((double)(srcN_w));
  
  //initialize the resizedImg
    Cube<pixel_type> resizedImg(target_h, target_w, 3);
    resizedImg.fill(0);

//fill in the resizedImg with the scaled value
    for (int x = 0; x < target_h; x++)
    {
        for (int y = 0; y < target_w; y++)
        {
            
            int x_original = round(x / scale_x);
            int y_original = round(y / scale_y);

            if (x_original > srcN_h) x_original = srcN_h-1;
            if (y_original > srcN_w) y_original = srcN_w -1;
            resizedImg.tube(x,y) = srcImg.tube(x_original,y_original);

        }
    }
    return resizedImg;
}

/**
 * @brief Find the best match based on the euclidean distance among the list of source images and return that specific img
 * 
 * @tparam pixel_type  the type of the pixel 
 * @param targetImg the target img we hope to compared with 
 * @param srcImg_List the list of sourced images 
 * @return Cube<pixel_type>  the img that we has the best match with the target img
 */
template <typename pixel_type>
Cube<pixel_type> getBestMatch(Cube<pixel_type> &targetImg, vector<Cube<pixel_type>> &srcImg_List){

    
    int argmax = 0;
    vector<float> targetAvgColor = getAvgColor(targetImg);
    double smallest_ed = std::numeric_limits<double>::infinity();

    for(int i = 0; i < (int)(srcImg_List.size()); i++){
        vector<float> srcAvgColor = getAvgColor(srcImg_List[i]);

        double ed = 0;
        for (int i =0; i < (int)(targetAvgColor.size()); i++){
            ed += pow((targetAvgColor.at(i) - srcAvgColor.at(i)),2);
        }
        ed = sqrt(ed);
        if (ed < smallest_ed){
            smallest_ed = ed;
            argmax = i;

        }


    }

    return srcImg_List[argmax];

}
/**
 * @brief Construct a new tuple<Cube<pixel type>, vector<float>>init tile object to construct a tile 
 *        The first element is the img of the tile and the second element is the avg color of that tile img
 * 
 * @tparam pixel_type The type of the pixel
 * @param srcImg The source img
 * @param img_out_ratio The img w/h
 * @param target_h  The height of the target img
 * @param target_w  The width of the target img
 */
template <typename pixel_type>
tuple<Cube<pixel_type>, vector<float>>init_tile(Cube<int>&srcImg, double img_out_ratio, int target_h, int target_w){
    Cube<pixel_type>cropped = maxCrop(srcImg, img_out_ratio);
    Cube<pixel_type>resizedImg = resize_image(cropped, target_h, target_w);
    vector<float> avgColor = getAvgColor(resizedImg);
    tuple<Cube<pixel_type>, vector<float>> result = {resizedImg, avgColor};
    return result;
    

}



