#ifndef DVD_IMAGE_H
#define DVD_IMAGE_H

#ifdef GAME
#include "file.h"
#endif
#ifndef COMPILER
#include "../renderer/texture2D.h"
#endif // COMPILER

#include <cstdint>
#include <string>
#include <vector>

/// @brief Image class ready for rendering
/// @details createFromFile assumes png but createFromMemory takes an
/// OpenGL-ready format
class Image {
public:
  enum {
    COLORTYPE_NONE,
    COLORTYPE_GRAYSCALE,
    COLORTYPE_GRAYSCALE_ALPHA,
    COLORTYPE_INDEXED,
    COLORTYPE_RGB,
    COLORTYPE_RGBA
  };

  enum class Render { NORMAL, ADDITIVE, SUBTRACTIVE, MULTIPLY };

public:
  Image();
  Image(Image &&other) noexcept;
  Image &operator=(Image &&other) noexcept;
  ~Image();

  // Do not copy images
  Image(const Image &other) = delete;
  Image &operator=(Image &other) = delete;

  unsigned int w, h;
#ifdef COMPILER
  uint8_t *data;
  int xpad;
  int ypad;
#else
  std::vector<renderer::Texture2D> textures;
  unsigned int w_textures;
  unsigned int h_textures;
  unsigned int w_subtexture;
  unsigned int h_subtexture;

  /// @brief Draw image on screen
  /// @typeparam T Specify shader_renderer
  template <typename T> void draw(int x, int y, bool mirror = false) const;

  /// @brief Draw image in relation to camera
  /// @typeparam T Specify shader_renderer
  template <typename T>
  void drawSprite(int x, int y, bool mirror = false) const;
#endif

  // DvD: Loads a RGB, RGBA, Greyscale, or GA png file
  // SPRTOOL/compiler: Loads a 256-color PNG from a file
  void createFromFile(std::string filename);
#ifdef GAME
  void createFromEmbed(File &file, const uint8_t *palette);
#endif
#ifndef COMPILER
  void createFromMemory(const uint8_t *data, unsigned int width,
                        unsigned int height, int format,
                        const uint8_t *palette);
#endif

  bool exists() const;

  static void setShake(bool _shake);

  static bool shake;
};

#endif // DVD_IMAGE_H
