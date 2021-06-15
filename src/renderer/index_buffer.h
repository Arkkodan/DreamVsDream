#ifndef RENDERER_INDEX_BUFFER_H
#define RENDERER_INDEX_BUFFER_H

#include "gl_loader.h"

namespace renderer {
  class IndexBuffer {
  public:
    IndexBuffer();
    ~IndexBuffer();

    void bind() const;

    void bindData(GLsizeiptr size, const void *data, GLsizei count,
                  GLenum usage, GLenum type);
    void bindQuadData(unsigned int quadCount, GLenum usage, GLenum type);

    void renderQuad() const;

  private:
    GLuint ibo;
    GLsizei count;
    GLenum type;
  };
} // namespace renderer

#endif // RENDERER_INDEX_BUFFER_H
