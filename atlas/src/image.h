#ifndef IMAGE_H_INCLUDED
#define IMAGE_H_INCLUDED

#include <stdint.h>
#include <string>

typedef char byte_t;
typedef unsigned char ubyte_t;

enum {
	PIXEL_NULL,
	PIXEL_INDEXED,
	PIXEL_RGB,
	PIXEL_RGBA,
	PIXEL_MAX
};

extern int channels[PIXEL_MAX];

class Image {
public:
	std::string name;
	ubyte_t atlas;
	uint16_t x;
	uint16_t y;
	uint16_t w;
	uint16_t h;
	uint16_t x_shift;
	uint16_t y_shift;
	ubyte_t* data;
};

int imageRead(std::string szFileName, struct Image* img, int type);
int imageReadPalette(std::string szFileName, unsigned char* palette);
void imageWrite(std::string szFileName, struct Image* img, int type, const unsigned char* palette);

#endif // IMAGE_H_INCLUDED
