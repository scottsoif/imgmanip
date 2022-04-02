#include <iostream>
#include <armadillo>
#include <boost/gil.hpp>
#include <boost/gil/io/io.hpp>
#include <boost/gil/extension/io/jpeg.hpp>

/*
for more gil documentation, see
"https://www.boost.org/doc/libs/1_77_0/libs/gil/doc/html/design/image_view.html"
 */

using namespace std;
using namespace arma;
using namespace boost::gil;

void imgio_hello_word() {
  cout << "hello world" << endl;
}

struct PixelGenerator{
        Cube<uint8_t>* storage;
        int next_i = 0, next_j = 0;
        int width = 0, height = 0;

        PixelGenerator(Cube<uint8_t>* storage):
          storage(storage),
          width(storage->n_cols),
          height(storage->n_rows)
          {}

         rgb8_pixel_t operator()() {

            auto ans =  rgb8_pixel_t{storage->at(next_i, next_j, 0),
                                storage->at(next_i, next_j, 1),
                                storage->at(next_i, next_j, 2)};
            next_j++;
            if (next_j == width) {
              next_j = 0;
              next_i++;
            }

          return ans;
          }
};

struct PixelReader{
        Cube<uint8_t>* storage;
        int next_i = 0, next_j = 0;
        int width = 0, height = 0;

        PixelReader(Cube<uint8_t>* storage):
          storage(storage),
          width(storage->n_cols),
          height(storage->n_rows)
          {}

        void operator()( rgb8_pixel_t p) {
                storage->at(next_i, next_j, 0) =  at_c<0>(p);
                storage->at(next_i, next_j, 1) =  at_c<1>(p);
                storage->at(next_i, next_j, 2) =  at_c<2>(p);

                next_j++;
                if (next_j == width) {
                  next_j = 0;
                  next_i++;
                }
        }
};

Cube<uint8_t> read_img(string filename) {
  rgb8_image_t img;
  image_read_settings<jpeg_tag> read_settings;
  read_image(filename, img, read_settings);
  Cube<uint8_t> storage(img.height(), img.width(), num_channels<rgb8_image_t>());
  for_each_pixel(const_view(img), PixelReader(&storage));
  return storage;
}

void write_img(Cube<uint8_t> &storage, string save_path) {
  rgb8_image_t writeimg(storage.n_cols, storage.n_rows);
  generate_pixels(view(writeimg), PixelGenerator(&storage));
  write_view(save_path, view(writeimg), jpeg_tag{});
}