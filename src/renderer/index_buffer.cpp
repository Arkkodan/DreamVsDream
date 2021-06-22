#include "index_buffer.h"

#include <vector>

renderer::IndexBuffer::IndexBuffer() : count(0), type(GL_UNSIGNED_INT) {
  glGenBuffers(1, &ibo);
}

renderer::IndexBuffer::~IndexBuffer() { glDeleteBuffers(1, &ibo); }

void renderer::IndexBuffer::bind() const {
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
}

void renderer::IndexBuffer::bindData(GLsizeiptr size, const void *data,
                                     GLsizei count, GLenum usage, GLenum type) {

  bind();

  glBufferData(GL_ELEMENT_ARRAY_BUFFER, size, data, usage);

  this->count = count;
  this->type = type;
}

void renderer::IndexBuffer::bindQuadData(unsigned int quadCount, GLenum usage,
                                         GLenum type) {
  switch (type) {
  case GL_UNSIGNED_INT: {
    // Using triangle strips to draw quads
    unsigned int vCount = quadCount * 4;
    std::vector<GLuint> elements;
    elements.reserve(vCount);
    for (GLuint index = 0; index < vCount; index += 4) {
      elements.push_back(index + 0);
      elements.push_back(index + 1);
      elements.push_back(index + 2);
      elements.push_back(index + 3);
    }
    bindData(vCount * sizeof(GLuint), elements.data(), vCount, usage, type);
    break;
  }
  default:
    // TODO: Error handling for unsupported type
    break;
  }
}

void renderer::IndexBuffer::renderQuad() const {
  glDrawElements(GL_TRIANGLE_STRIP, count, type, nullptr);
}
