#ifndef RENDERER_VERTEX_BUFFER_H
#define RENDERER_VERTEX_BUFFER_H

#include "gl_loader.h"

namespace renderer {
  class VertexBuffer {
  public:
    VertexBuffer();
    ~VertexBuffer();

    void bind() const;

    void bindData(GLsizeiptr size, const void *data, GLenum usage) const;

  private:
    GLuint vbo;
  };
} // namespace renderer

#endif // RENDERER_VERTEX_BUFFER_H
