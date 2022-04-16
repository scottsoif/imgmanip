// module;
#include <iostream>
// #include <algorithm>
#include <armadillo>
#include <vector>
#include "imgio/imgio.h"
#include <chrono>
#include <string>
#include <filesystem>

using namespace std;
using namespace arma;
namespace fs = std::filesystem;

typedef std::chrono::duration<double, std::milli> millisec_type;

template<typename pixel_type>
void coutImgAttr(Cube<pixel_type> &img){
    cout << "height: " << img.n_rows << endl;
    cout << "width: " << img.n_cols << endl;
}

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
        for (int j =0; j < (int)(targetAvgColor.size()); j++){
            ed += pow((targetAvgColor.at(j) - srcAvgColor.at(j)),2);
        }
        ed = sqrt(ed);
        if (ed < smallest_ed){
            smallest_ed = ed;
            argmax = i;

        }
    }

    return srcImg_List[argmax];

}

template <typename pixel_type>
int getBestMatchIdx(vector<float> targetAvgColor, vector<vector<float>> &srcAvgColors){

    int argmax = 0;
    double smallest_ed = std::numeric_limits<double>::infinity();

    for(int i = 0; i < (int)(srcAvgColors.size()); i++){
        vector<float> srcAvgColor = srcAvgColors[i];

        double ed = 0;
        for (int j =0; j < (int)(targetAvgColor.size()); j++){
            ed += pow((targetAvgColor.at(j) - srcAvgColor.at(j)),2);
    }
        ed = sqrt(ed);
        if (ed < smallest_ed){
            smallest_ed = ed;
            argmax = i;

        }
    }

    return argmax;
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

/**
 * @brief fill the canvas in the region starting from (left_top_r, left_top_c) with the pixels
 *         from tiles
 *
 * @tparam pixel_type The type of the pixel
 * @param canvas canvas to fill on
 * @param tile source tile image
 * @param left_top_r the starting pixel coordinate r on canvas
 * @param left_top_c the starting pixel coordinate c on canvas
 */
template<typename pixel_type>
void fill_image(Cube<pixel_type> &canvas, Cube<pixel_type> &tile, int left_top_r, int left_top_c){
    canvas(span(left_top_r, left_top_r + tile.n_rows - 1), span(left_top_c, left_top_c + tile.n_cols - 1), span::all) = tile;
}

template<typename pixel_type>
bool isTileDense(Cube<pixel_type> &srcImg, int tile_h, int tile_w) {
    return (int)srcImg.n_rows >= tile_h && (int)srcImg.n_cols >= tile_w;
}

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
template<typename pixel_type>
Cube<pixel_type> create_mosaic(string tgt_img_path, string src_img_dir, int tile_cnt_h, int tile_cnt_w){

    Cube<pixel_type>tgt_img = read_img<pixel_type>(tgt_img_path);
    vector<Cube<pixel_type>> src_imgs;

    // n_rows = height, n_cols = width
    int mosaic_cnt = tile_cnt_h * tile_cnt_w;
    int tile_h = tgt_img.n_rows / tile_cnt_h;
    int tile_w = tgt_img.n_cols / tile_cnt_w;
    int tgt_h = tile_h * tile_cnt_h;
    int tgt_w = tile_w * tile_cnt_w;
    double tgt_wh_ratio = tgt_w / (double)tgt_h;


    cout << "orig tgt h, orig tgt w, tgt h, tgt w, tile h, tile w, tgt_wh_ratio" << endl <<
    tgt_img.n_rows << "," << tgt_img.n_cols<< ","  << tgt_h<< ","  << tgt_w << ","
    << tile_h << "," << tile_w << tgt_wh_ratio << endl;



    tgt_img = maxCrop(tgt_img, tgt_wh_ratio);

    for (const auto & entry : fs::directory_iterator(src_img_dir)) {
        cout << entry.path() << endl;
        cout << get_file_extension(entry.path()) << endl;
        cout << is_file_img(entry.path()) << endl;

        if (!is_file_img(entry.path())) continue;
        Cube<pixel_type> orig_src_img;

        try {
           orig_src_img = read_img<pixel_type>(entry.path());
        } catch (ios_base::failure const&) {
            cerr << "image is not loaded: " << entry.path() << endl;
            continue;
        }
        cout << "orig_src_img" << endl;
        coutImgAttr(orig_src_img);


        auto crop_src_img = maxCrop(orig_src_img, tile_w / tile_h);
        cout << "crop_src_img" << endl;
        coutImgAttr(crop_src_img);

        if (!isTileDense(crop_src_img, tile_h, tile_w)) continue;

        auto rsz_src_img = resize_image(crop_src_img, tile_h, tile_w);
        cout << "rsz_src_img" << endl;
        coutImgAttr(rsz_src_img);

        src_imgs.push_back(rsz_src_img);
        if (static_cast<int>(src_imgs.size()) == mosaic_cnt) break;
    }
    cout << "finish loading src img list" << endl;

    Cube<pixel_type> canvas_img(tgt_h, tgt_w, tgt_img.n_slices);

    // iteratre through each tile of the tgt_img and canvas_img
    int right_bot_r = tile_h, right_bot_c = tile_w;
    int left_top_r = right_bot_r - tile_h, left_top_c = right_bot_c - tile_w;

    string tgt_img_fname = "imgs/test_tiles/tgt_img.jpg";


    while (right_bot_r <= tgt_h && right_bot_c <= tgt_w) {

        cout << "============================tile loop============================" << endl;

        cout << "target img attribute: ";
        coutImgAttr(tgt_img);
        cout << "left_top_r, left_top_c, right_bot_r, right_bot_c "
        << left_top_r << ", " << left_top_c << ", "
        << right_bot_r << ", " << right_bot_c << endl;

        Cube<pixel_type>tgt_tile = crop(tgt_img, left_top_r, left_top_c, tile_w, tile_h);
        cout << "cropped" << endl;
        Cube<pixel_type>canvas_tile = getBestMatch(tgt_tile, src_imgs);
        cout << "gotbestmatch" << endl;

        // for debugging purpose
        // string tgt_tile_fname = "imgs/test_tiles/tgt_tile_" + to_string(tile_cnt) + ".jpg";
        // string src_tile_fname = "imgs/test_tiles/src_tile_" + to_string(tile_cnt++) + ".jpg";
        // write_img(tgt_tile, tgt_tile_fname);
        // write_img(canvas_tile, src_tile_fname);

        cout << "tgt_tile and canvas_tile created" << endl;

        fill_image(canvas_img, canvas_tile, left_top_r, left_top_c);

        // iteration update
        right_bot_c += tile_w;
        if (right_bot_c > tgt_w) {
            right_bot_r += tile_h;
            right_bot_c = tile_w;
        }
        left_top_r = right_bot_r - tile_h, left_top_c = right_bot_c - tile_w;
    }

    return canvas_img;
}


