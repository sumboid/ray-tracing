#include "bitmap.h"


int main(int argc, char** argv) {
  char* begin = argv[1];
  int width = bitmap_image(begin).width();

  bitmap_image result(width, width);

  for(int i = 1; i < argc; ++i) {
    bitmap_image img(argv[i]);
    for(int  j = 0; j < width; ++j) {
      unsigned char c[3];
      img.get_pixel(j, 0, c[0], c[1], c[2]);
      result.set_pixel(j, i - 1, c[0], c[1], c[2]);
    }
  }

  result.save_image("result.bmp");
  return 0;
}
