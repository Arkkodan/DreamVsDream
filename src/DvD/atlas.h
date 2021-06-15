#ifndef DVD_ATLAS_H
#define DVD_ATLAS_H

#include "file.h"
#include "image.h"

#include <cstdint>
#include <vector>

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
  Atlas(Atlas &&other) noexcept;
  Atlas &operator=(Atlas &&other) noexcept;
  ~Atlas();

  // Do not copy atlases
  Atlas(const Atlas &other) = delete;
  Atlas &operator=(Atlas &other) = delete;

  bool create(File &file, const uint8_t *palette);

  void draw(int sprite, int x, int y, bool mirror) const;
  void drawSprite(int sprite, int x, int y, bool mirror) const;

  AtlasSprite getSprite(int sprite) const;

private:
  int nSprites;
  std::vector<AtlasSprite> sprites;

  int nImages;
  std::vector<Image> images;
};

#endif // DVD_ATLAS_H
