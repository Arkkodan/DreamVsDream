#include "atlas.h"

#include "graphics.h"

// #define TEXTURE_SIZE 2048
// #define TEXTURE_SIZE_SQ (TEXTURE_SIZE*TEXTURE_SIZE)

Atlas::Atlas() { nImages = nSprites = 0; }

Atlas::Atlas(Atlas &&other) noexcept {
  nSprites = other.nSprites;
  sprites = std::move(other.sprites);

  nImages = other.nImages;
  images = std::move(other.images);
}

Atlas &Atlas::operator=(Atlas &&other) noexcept {
  nSprites = other.nSprites;
  nImages = other.nImages;

  images.swap(other.images);
  sprites.swap(other.sprites);

  return *this;
}

Atlas::~Atlas() {
  // if(textures) glDeleteTextures(c_textures, textures);
}

bool Atlas::create(File &file, const uint8_t *palette) {
  (void)palette;

  // Setup sprite buffers
  nSprites = file.readWord();
  sprites.resize(nSprites);

  // Read sprite info
  for (int i = 0; i < nSprites; i++) {
    sprites[i].atlas = file.readByte();
    sprites[i].x = file.readWord();
    sprites[i].y = file.readWord();
    sprites[i].w = file.readWord();
    sprites[i].h = file.readWord();
  }

  // Read images
  nImages = file.readByte();
  images.resize(nImages);
  for (int i = 0; i < nImages; i++) {
    images[i].createFromEmbed(file, palette);
  }

  return true;
}

void Atlas::draw(int sprite_, int x_, int y_, bool mirror_) const {
  graphics::setRect(sprites[sprite_].x, sprites[sprite_].y, sprites[sprite_].w,
                    sprites[sprite_].h);
  images[sprites[sprite_].atlas].draw(x_, y_, mirror_);
}

void Atlas::drawSprite(int sprite_, int x_, int y_, bool mirror_) const {
  graphics::setRect(sprites[sprite_].x, sprites[sprite_].y, sprites[sprite_].w,
                    sprites[sprite_].h);
  images[sprites[sprite_].atlas].drawSprite(x_, y_, mirror_);
}

AtlasSprite Atlas::getSprite(int sprite_) const {
  if (sprite_ < 0 || sprite_ >= nSprites) {
    return sprites[0];
  }
  return sprites[sprite_];
}

AtlasSprite::AtlasSprite() {
  atlas = 0;
  x = y = w = h = 0;
}
