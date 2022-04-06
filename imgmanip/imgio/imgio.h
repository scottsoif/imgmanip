#include <armadillo>
#include <iostream>
#include <boost/gil.hpp>
#include <boost/gil/io/io.hpp>
#include <boost/gil/extension/io/jpeg.hpp>

using namespace std;
using namespace arma;

void imgio_hello_word();
Cube<uint8_t> read_img(string);
void write_img(Cube<uint8_t>&, string);


/*
for more gil documentation, see
"https://www.boost.org/doc/libs/1_77_0/libs/gil/doc/html/design/image_view.html"
 */

void imgio_hello_word() {
  cout << "hello world" << endl;
}

template <typename pixel_type>
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

template <typename pixel_type>
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

template <typename pixel_type>
Cube<pixel_type> read_img(string filename) {
  using namespace boost::gil;
  rgb8_image_t img;
  image_read_settings<jpeg_tag> read_settings;
  read_image(filename, img, read_settings);
  Cube<pixel_type> storage(img.height(), img.width(), num_channels<rgb8_image_t>());
  for_each_pixel(const_view(img), PixelReader(&storage));
  return storage;
}

template <typename pixel_type>
void write_img(Cube<pixel_type> &storage, string save_path) {
  using namespace boost::gil;
  rgb8_image_t writeimg(storage.n_cols, storage.n_rows);
  generate_pixels(view(writeimg), PixelGenerator(&storage));
  write_view(save_path, view(writeimg), jpeg_tag{});
}