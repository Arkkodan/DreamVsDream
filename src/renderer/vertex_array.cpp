#include "vertex_array.h"

renderer::VertexArray::VertexArray() { glGenVertexArrays(1, &vao); }

renderer::VertexArray::~VertexArray() { glDeleteVertexArrays(1, &vao); }

void renderer::VertexArray::bind() const { glBindVertexArray(vao); }
