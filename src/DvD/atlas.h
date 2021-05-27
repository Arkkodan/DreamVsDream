#ifndef ATLAS_H_INCLUDED
#define ATLAS_H_INCLUDED

#include "globals.h"

#include "image.h"
#include "file.h"

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
	Atlas(Atlas&& other);
	Atlas& operator=(Atlas&& other);
	~Atlas();

	//Do not copy atlases
	Atlas(const Atlas& other) = delete;
	Atlas& operator=(Atlas& other) = delete;

	bool create(File& file, const ubyte_t* palette);

	void draw(int sprite, int x, int y, bool mirror);
	void drawSprite(int sprite, int x, int y, bool mirror);

	AtlasSprite getSprite(int sprite);

private:
	int nSprites;
	AtlasSprite* sprites;

	int nImages;
	Image* images;
};

#endif // ATLAS_H_INCLUDED
