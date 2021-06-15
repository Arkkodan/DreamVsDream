#include "texture2D.h"

renderer::Texture2D::Texture2D() : width(0), height(0) {
  glGenTextures(1, &texture);
}

renderer::Texture2D::~Texture2D() { glDeleteTextures(1, &texture); }

void renderer::Texture2D::bind() const {
  glBindTexture(GL_TEXTURE_2D, texture);
}

void renderer::Texture2D::bindData(GLsizei width, GLsizei height, GLenum format,
                                   GLenum type, const void *buffer) {
  bind();
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, format, type,
               buffer);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

  this->width = width;
  this->height = height;
}

void renderer::Texture2D::activate(unsigned int textureOffset) const {
  glActiveTexture(GL_TEXTURE0 + textureOffset);
  bind();
}

GLuint renderer::Texture2D::getTexture() const { return texture; }

GLsizei renderer::Texture2D::getWidth() const { return width; }

GLsizei renderer::Texture2D::getHeight() const { return height; }
