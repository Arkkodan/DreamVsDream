#include "font.h"

#include "../util/fileIO.h"
#include "error.h"
#include "file.h"
#include "graphics.h"
#include "shader_renderer/texture2D_renderer.h"

float Font::xscale = 1.0f;
float Font::yscale = 1.0f;

Font::Font() : pos(), width(), mono(0), sensitive(false) {}

Font::~Font() {}

void Font::createFromFile(std::string filename) {
  File file;
  std::string path = util::getPath(filename);
  if (!file.open(File::FILE_READ_NORMAL, path)) {
    error::error("Unable to load font \"" + path + "\".");
    return;
  }

  // Read options
  mono = file.readByte();
  sensitive = file.readByte();

  uint8_t c;
  uint16_t p;
  uint8_t w;
  for (int i = 0;; i++) {
    c = file.readByte();
    if (c == 0) {
      break;
    }
    if (mono) {
      pos[c] = i * mono;
    }
    else {
      p = file.readWord();
      w = file.readByte();
      pos[c] = p;
      width[c] = w;
    }
  }
  img.createFromEmbed(file, nullptr);
}

bool Font::exists() const { return img.exists(); }

void Font::drawText(int x, int y, std::string text) const {
  drawText(x, y, text, 255, 255, 255, 1.0f);
}

void Font::drawChar(int x, int y, char c) const {
  drawChar(x, y, c, 255, 255, 255, 1.0f);
}

void Font::drawChar(int x, int y, char c, uint8_t r, uint8_t g, uint8_t b,
                    float a) const {
  if (!sensitive) {
    if (c >= 'A' && c <= 'Z') {
      c += 'a' - 'A';
    }
  }
  renderer::Texture2DRenderer::setColor(
      {r / 255.0f, g / 255.0f, b / 255.0f, a});
  unsigned imgH = img.getH();
  uint8_t uc = static_cast<uint8_t>(c);
  if (mono) {
    graphics::setRect(pos[uc], 0, mono, imgH);
  }
  else {
    graphics::setRect(pos[uc], 0, width[uc], imgH);
  }
  graphics::setScale(xscale, yscale);
  img.draw<renderer::Texture2DRenderer>(x, y);
  xscale = 1.0f;
  yscale = 1.0f;
}

void Font::drawText(int x, int y, std::string text, uint8_t r, uint8_t g,
                    uint8_t b, float a) const {
  if (!text.length()) {
    return;
  }

  int origX = x;
  unsigned int imgH = img.getH();
  float fx = static_cast<float>(x), fy = static_cast<float>(y);

  for (int i = 0; text[i]; i++) {
    if (text[i] == ' ') {
      if (mono) {
        fx += mono * xscale;
      }
      else {
        fx += 7 * xscale;
      }
    }
    else if (text[i] == '\n') {
      fx = static_cast<float>(origX);
      fy += imgH * yscale;
    }
    else if (text[i] == '\t') {
      fx += (imgH - (static_cast<int>(fx) - origX) % imgH) * xscale;
    }
    else {
      char c = text[i];
      if (!sensitive) {
        if (c >= 'A' && c <= 'Z') {
          c += 'a' - 'A';
        }
      }
      renderer::Texture2DRenderer::setColor(
          {r / 255.0f, g / 255.0f, b / 255.0f, a});
      uint8_t uc = static_cast<uint8_t>(c);
      if (mono) {
        graphics::setRect(pos[uc], 0, mono, imgH);
      }
      else {
        graphics::setRect(pos[uc], 0, width[uc], imgH);
      }
      graphics::setScale(xscale, yscale);
      img.draw<renderer::Texture2DRenderer>(static_cast<int>(fx),
                                            static_cast<int>(fy));
      if (mono) {
        fx += mono * xscale;
      }
      else {
        fx += (width[uc] + 1) * xscale;
      }
    }
  }

  xscale = 1.0f;
  yscale = 1.0f;
}

int Font::getTextWidth(std::string text) const {
  if (!mono) {
    int w = 0;
    for (const char *sz = text.c_str(); *sz; sz++) {
      w += getCharWidth(*sz);
    }
    return w;
  }
  return static_cast<int>(text.length() * mono * xscale);
}

int Font::getCharWidth(char c) const {
  if (mono) {
    return mono;
  }
  if (c == ' ') {
    return static_cast<int>(7 * xscale);
  }
  return static_cast<int>((width[(uint8_t)c] + 1) * xscale);
}

void Font::setScale(float _xscale, float _yscale) {
  if (!_yscale) {
    _yscale = _xscale;
  }
  xscale = _xscale;
  yscale = _yscale;
}

const Image *Font::getcImage() const { return &img; }
