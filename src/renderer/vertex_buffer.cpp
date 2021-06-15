#include "vertex_buffer.h"

renderer::VertexBuffer::VertexBuffer() { glGenBuffers(1, &vbo); }

renderer::VertexBuffer::~VertexBuffer() { glDeleteBuffers(1, &vbo); }

void renderer::VertexBuffer::bind() const {
  glBindBuffer(GL_ARRAY_BUFFER, vbo);
}

void renderer::VertexBuffer::bindData(GLsizeiptr size, const void *data,
                                      GLenum usage) const {
  bind();
  glBufferData(GL_ARRAY_BUFFER, size, data, usage);
}
