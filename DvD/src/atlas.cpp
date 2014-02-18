#include "atlas.h"

#include <stdlib.h>

#include "globals.h"
#include "file.h"
#include "stage.h"
#include "graphics.h"

#define TEXTURE_SIZE 2048
#define TEXTURE_SIZE_SQ (TEXTURE_SIZE*TEXTURE_SIZE)

Atlas::Atlas() {
	nImages = nSprites = 0;
	images = nullptr;
	sprites = nullptr;
}

Atlas::Atlas(Atlas&& other) {
    nSprites = other.nSprites;
    sprites = other.sprites;

    nImages = other.nImages;
    images = other.images;
    other.sprites = nullptr;
    other.images = nullptr;
}

Atlas& Atlas::operator=(Atlas&& other) {
    nSprites = other.nSprites;
    nImages = other.nImages;

    using std::swap;
    swap(images, other.images);
    swap(sprites, other.sprites);

    return *this;
}

Atlas::~Atlas() {
	//if(textures) glDeleteTextures(c_textures, textures);

	delete [] sprites;
	delete [] images;
}

bool Atlas::create(File& file, const ubyte_t* palette) {
	(void)palette;
	
	//Setup sprite buffers
	nSprites = file.readWord();
	sprites = new AtlasSprite[nSprites];

	//Read sprite info
	for(int i = 0; i < nSprites; i++) {
		sprites[i].atlas = file.readByte();
		sprites[i].x = file.readWord();
		sprites[i].y = file.readWord();
		sprites[i].w = file.readWord();
		sprites[i].h = file.readWord();
	}
	
	//Read images
	nImages = file.readByte();
	images = new Image[nImages];
	for(int i = 0; i < nImages; i++) {
		images[i].createFromEmbed(file, palette);
	}
	
	return true;
}

void Atlas::draw(int sprite_, int x_, int y_, bool mirror_) {
	graphics::setRect(sprites[sprite_].x, sprites[sprite_].y, sprites[sprite_].w, sprites[sprite_].h);
	images[sprites[sprite_].atlas].draw(x_, y_, mirror_);
}

void Atlas::drawSprite(int sprite_, int x_, int y_, bool mirror_) {
	graphics::setRect(sprites[sprite_].x, sprites[sprite_].y, sprites[sprite_].w, sprites[sprite_].h);
	images[sprites[sprite_].atlas].drawSprite(x_, y_, mirror_);
}

AtlasSprite Atlas::getSprite(int sprite_) {
	if(sprite_ < 0 || sprite_ >= nSprites) {
		return sprites[0];
	}
	return sprites[sprite_];
}

AtlasSprite::AtlasSprite() {
	atlas = 0;
	x = y = w = h = 0;
}
