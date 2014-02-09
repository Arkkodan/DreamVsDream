#ifndef IMAGE_H_INCLUDED
#define IMAGE_H_INCLUDED

#include "globals.h"

enum {
	COLORTYPE_NONE,
	COLORTYPE_GRAYSCALE,
	COLORTYPE_GRAYSCALE_ALPHA,
	COLORTYPE_INDEXED,
	COLORTYPE_RGB,
	COLORTYPE_RGBA
};

enum {
	RENDER_NORMAL,
	RENDER_ADDITIVE,
	RENDER_SUBTRACTIVE,
	RENDER_MULTIPLY
};

class Image {
public:
	Image();
	Image(Image&& other);
	Image& operator=(Image&& other);
	~Image();

	//Do not copy images
	Image(const Image& other) = delete;
	Image& operator=(Image& other) = delete;

	unsigned int w, h;
#ifdef COMPILER
	ubyte_t* data;
	int xpad;
	int ypad;
#else
	unsigned int* textures;
	unsigned int w_textures;
	unsigned int h_textures;
	unsigned int w_subtexture;
	unsigned int h_subtexture;

	void draw(int x, int y, bool mirror = false);
	void drawSprite(int x, int y, bool mirror = false);
#endif

	//DvD: Loads a RGB, RGBA, Greyscale, or GA png file
	//SPRTOOL/compiler: Loads a 256-color PNG from a file
	void createFromFile(std::string filename);
#ifndef COMPILER
	void createFromMemoryPNG(const ubyte_t* imgdata, size_t size);
	void createFromMemory(const ubyte_t* data, unsigned int width, unsigned int height, int format);
#endif

	bool exists();

	static void setShake(bool _shake);

	static bool shake;

	static const ubyte_t palTransFirst[256];
};

#endif // IMAGE_H_INCLUDED
