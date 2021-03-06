#ifndef DVD_GRAPHICS_H
#define DVD_GRAPHICS_H

#ifndef COMPILER

#include "image.h"

#include "../renderer/texture2D.h"

#include <glm/mat4x4.hpp>

#include <cstdint>

namespace graphics {
  /// @brief Set some OpenGL variables, load procedures, check support
  void init(bool disable_shaders, unsigned int max_texture_size);
  /// @brief Deinit
  void deinit();

  /// @brief Apply shader effects and wait for 1 frame
  void refresh();

  /// @brief Primitive clear color
  void setClearColor(uint8_t r, uint8_t g, uint8_t b);
  /// @brief Primitive rectangle
  void setRect(int sX, int sY, int sW, int sH);
  /// @brief Primitive scaling
  void setScale(float scale, float yscale = 0.0f);
  /// @brief Primitive render
  void setRender(Image::Render render);

  /// @brief Input data into shader
  void setPalette(const renderer::Texture2D &palette, float alpha, float r,
                  float g, float b, float pct);

  Image::Render getRender();

  int &getrSourceX();
  int &getrSourceY();
  int &getrSourceW();
  int &getrSourceH();

  float &getrXScale();
  float &getrYScale();

  unsigned int getMaxTextureSize();
  bool hasShaderSupport();

  const glm::mat4x4 &getcrProjectionMatrix();
} // namespace graphics

#endif // COMPILER

#endif // DVD_GRAPHICS_H
