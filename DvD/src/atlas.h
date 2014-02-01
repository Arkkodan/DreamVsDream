#ifndef ATLAS_H_INCLUDED
#define ATLAS_H_INCLUDED

#include "image.h"
#include "util.h"

enum {
	PIXEL_NULL,
	PIXEL_INDEXED,
	PIXEL_RGB,
	PIXEL_RGBA,
	PIXEL_MAX
};

//Describes a sprite
class AtlasSprite {
public:
	AtlasSprite();

	int atlas;
	int x, y, w, h;
};

//ATLAS - A collection of sprites
class Atlas {
public:

	Atlas();
	~Atlas();

	bool create(std::string szFilename);
	bool createFromPalette(std::string szFilename, const ubyte_t* palette);

	void draw(int sprite, int x, int y, bool mirror);
	void drawSprite(int sprite, int x, int y, bool mirror);

	AtlasSprite getSprite(int sprite);

	static const int channels[PIXEL_MAX];

private:
	int pixel_type;

	int nSprites;
	AtlasSprite* sprites;

	int nImages;
	Image* images;
};

#endif // ATLAS_H_INCLUDED
