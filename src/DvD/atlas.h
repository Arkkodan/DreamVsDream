#ifndef ATLAS_H_INCLUDED
#define ATLAS_H_INCLUDED

#include "image.h"
#include "file.h"

/// @brief Data structure representing a sprite in an atlas
class AtlasSprite {
public:
	AtlasSprite();

	int atlas;
	int x, y, w, h;
};

/// @brief Atlas class representing a collection of sprites in crammed images
class Atlas {
public:

	Atlas();
	Atlas(Atlas&& other);
	Atlas& operator=(Atlas&& other);
	~Atlas();

	//Do not copy atlases
	Atlas(const Atlas& other) = delete;
	Atlas& operator=(Atlas& other) = delete;

	bool create(File& file, const uint8_t* palette);

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
