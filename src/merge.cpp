#include "bitmap.h"


int main(int argc, char** argv) {
  char* begin = argv[1];
  int width = bitmap_image(begin).width();

  bitmap_image result(width, width);

  int gh = 0;
  for(int i = 1; i < argc; ++i) {
    bitmap_image img(argv[i]);
    int height = img.height();
    for(int k = 0; k < height; ++k) {
      for(int  j = 0; j < width; ++j) {
        unsigned char c[3];
        img.get_pixel(j, k, c[0], c[1], c[2]);
        result.set_pixel(j, gh, c[0], c[1], c[2]);
      }
      gh++;
    }
  }

  result.save_image("result.bmp");
  return 0;
}
