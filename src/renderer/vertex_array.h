#ifndef RENDERER_VERTEX_ARRAY_H
#define RENDERER_VERTEX_ARRAY_H

#include "gl_loader.h"

namespace renderer {
  class VertexArray {
  public:
    VertexArray();
    ~VertexArray();

    void bind() const;

  private:
    GLuint vao;
  };
} // namespace renderer

#endif // RENDERER_VERTEX_ARRAY_H
