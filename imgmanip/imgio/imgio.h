#ifndef IMGIO_H_
#define IMGIO_H_

// #include <armadillo>
#include <iostream>
#include <functional>
#include <type_traits>

#include "pch.h"
// #include <boost/gil.hpp>
// #include <boost/gil/io/io.hpp>
// #include <boost/gil/extension/io/jpeg.hpp>
// #include <boost/gil/extension/io/png.hpp>

using namespace std;
using namespace arma;

template<typename T>
concept NumericType = requires(T param)
{
    requires is_integral_v<T> || is_floating_point_v<T>;
    requires !is_same_v<bool, T>;
    requires is_arithmetic_v<decltype(param +1)>;
    requires !is_pointer_v<T>;
};

template <NumericType pixel_type>
Cube<pixel_type> read_img(string);

template <NumericType pixel_type>
void write_img(Cube<pixel_type>&, string);

/*
for more gil documentation, see
"https://www.boost.org/doc/libs/1_77_0/libs/gil/doc/html/design/image_view.html"
 */

template <NumericType pixel_type>
struct PixelGenerator{

  Cube<pixel_type>* storage;
  int next_i = 0, next_j = 0;
  int width = 0, height = 0;

  PixelGenerator(Cube<pixel_type>* storage):
    storage(storage),
    width(storage->n_cols),
    height(storage->n_rows)
    {}

    boost::gil::rgb8_pixel_t operator()() {

      auto ans =  boost::gil::rgb8_pixel_t{
                          (uint8_t)storage->at(next_i, next_j, 0),
                          (uint8_t)storage->at(next_i, next_j, 1),
                          (uint8_t)storage->at(next_i, next_j, 2)
      };

      next_j++;
      if (next_j == width) {
        next_j = 0;
        next_i++;
      }

    return ans;
    }
};

template <NumericType pixel_type>
struct PixelReader{
  Cube<pixel_type>* storage;
  int next_i = 0, next_j = 0;
  int width = 0, height = 0;

  PixelReader(Cube<pixel_type>* storage):
    storage(storage),
    width(storage->n_cols),
    height(storage->n_rows)
    {}

  void operator()(boost::gil::rgb8_pixel_t p) {
          storage->at(next_i, next_j, 0) = (pixel_type)boost::gil::at_c<0>(p);
          storage->at(next_i, next_j, 1) = (pixel_type)boost::gil::at_c<1>(p);
          storage->at(next_i, next_j, 2) = (pixel_type)boost::gil::at_c<2>(p);

          next_j++;
          if (next_j == width) {
            next_j = 0;
            next_i++;
          }
  }
};
/**
 * @brief Get the extention of the image file  (jpeg, jpg, png)
 * 
 * @param filename the name of the image file
 * @return string the extention of the img file
 */
string get_file_extension(string filename) {
  return filename.substr(filename.find_last_of(".") + 1);
}
/**
 * @brief Return whether the file is an image or not
 * 
 * @param filename the name of the image file
 * @return whether the file is an image or not
 * 
 */
bool is_file_img(string filename) {
  string ext = get_file_extension(filename);
  return ext == "jpeg" || ext == "jpg" || ext == "png";
}

/**
 * @brief Read the image
 * 
 * @tparam pixel_type the type of the pixel
 * @param filename the name of the image file
 * @return Cube<pixel_type> the cube of the image representation
 */
template <NumericType pixel_type>
Cube<pixel_type> read_img(string filename) {
  using namespace boost::gil;
  rgb8_image_t img;

  string file_ext = get_file_extension(filename);

  if (file_ext == "jpeg" || file_ext == "jpg")
    read_and_convert_image(filename, img, jpeg_tag{});
  else if (file_ext == "png")
    read_and_convert_image(filename, img, png_tag{});
  else
    throw "input image format is not supported!";

  Cube<pixel_type> storage(img.height(), img.width(), num_channels<rgb8_image_t>());
  for_each_pixel(const_view(img), PixelReader(&storage));

  return storage;
}
/**
 * @brief Write from cube to an image
 * 
 * @tparam pixel_type the type of the pixel
 * @param storage the cube of the image representation
 * @param save_path the path to save the generated image
 */
template <NumericType pixel_type>
void write_img(Cube<pixel_type> &storage, string save_path) {
  using namespace boost::gil;
  rgb8_image_t writeimg(storage.n_cols, storage.n_rows);

  string save_ext = get_file_extension(save_path);
  generate_pixels(view(writeimg), PixelGenerator(&storage));

  if (save_ext == "jpeg" || save_ext == "jpg")
    write_view(save_path, view(writeimg), jpeg_tag{});
  else if (save_ext == "png")
    write_view(save_path, view(writeimg), png_tag{});
  else
    throw "saving image format is not supported!";
}

/**
 * @brief Read more than one images
 * 
 * @tparam pixel_type the type of the pixel
 * @param filenames the name of the image files
 * @param pipeline read in image pipeline function
 * @return vector<Cube<pixel_type>> the vector of cube representations in images
 */
template <NumericType pixel_type>
vector<Cube<pixel_type>> read_many_imgs(vector<string>& filenames, function<Cube<pixel_type>(Cube<pixel_type>)> pipeline) {
  vector<Cube<pixel_type>> imgs;
  for (string &file : filenames) {
    try {
      Cube<pixel_type> orig_img = read_img<pixel_type>(file);
      imgs.push_back(pipeline(orig_img));
    } catch (ios_base::failure const&) {
      cerr << "image is not loaded: " << file << endl;
      continue;
    }
  }
  return imgs;
}

#endif