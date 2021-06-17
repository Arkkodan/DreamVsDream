#ifndef DVD_FONT_H
#define DVD_FONT_H

#include "image.h"

#include <array>
#include <cstdint>
#include <string>

/// @brief Font class for rendering
/// @details Uses Dream vs. Dream's font format
class Font {
public:
  static void setScale(float xscale, float yscale = 0.0f);

public:
  Font();
  // Font(std::string filename);
  ~Font();

  void drawChar(int x, int y, char c) const;
  void drawChar(int x, int y, char c, uint8_t r, uint8_t g, uint8_t b,
                float a = 1.0f) const;
  void drawText(int x, int y, std::string text) const;
  void drawText(int x, int y, std::string text, uint8_t r, uint8_t g, uint8_t b,
                float a = 1.0f) const;
  int getTextWidth(std::string text) const;
  int getCharWidth(char c) const;

  void createFromFile(std::string filename);
  bool exists() const;

  const Image *getcImage() const;

private:
  static float xscale;
  static float yscale;

private:
  Image img;
  std::array<uint16_t, 256> pos;
  std::array<char, 256> width;

  int mono;       // Size of monospace characters; 0 is variable width
  bool sensitive; // Case sensitive?
};

#endif // DVD_FONT_H
