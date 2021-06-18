#ifndef RENDERER_TEXTURE2D_H
#define RENDERER_TEXTURE2D_H

#include "gl_loader.h"

namespace renderer {
  class Texture2D {
  public:
    Texture2D();
    ~Texture2D();

    void bind() const;

    // Bind data to this Texture2D object onto the GPI, buffer can be freed
    // after this
    void bindData(GLsizei width, GLsizei height, GLenum format, GLenum type,
                  const void *buffer);

    // Set texture as active, offset to GL_TEXTURE0
    void activate(unsigned int textureOffset) const;

    GLuint getTexture() const;

    GLsizei getWidth() const;
    GLsizei getHeight() const;

  private:
    GLuint texture;

    GLsizei width;
    GLsizei height;
  };
} // namespace renderer

#endif // RENDERER_TEXTURE2D_H
