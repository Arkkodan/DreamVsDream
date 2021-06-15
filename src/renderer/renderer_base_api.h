#ifndef RENDERER_RENDERER_BASE_API_H
#define RENDERER_RENDERER_BASE_API_H

#include "gl_loader.h"

#include "index_buffer.h"
#include "shader.h"
#include "vertex_array.h"
#include "vertex_buffer.h"

#include <memory>

namespace renderer {
  class Renderer {
  protected:
    // Suggested to access by singleton (dynamic_cast Renderer *)
    // static std::unique_ptr<Renderer> s_renderer;

  protected:
    // Do not call constructor or destructor manually
    Renderer() {
      vertexArray = std::make_unique<VertexArray>();
      vertexArray->bind();
      vertexBuffer = std::make_unique<VertexBuffer>();
      vertexBuffer->bind();
      indexBuffer = std::make_unique<IndexBuffer>();
      indexBuffer->bind();
    };
    virtual ~Renderer(){};

  public:
    static void draw() = delete;

  protected:
    std::unique_ptr<VertexArray> vertexArray;
    std::unique_ptr<VertexBuffer> vertexBuffer;
    std::unique_ptr<IndexBuffer> indexBuffer;

    std::unique_ptr<Shader> vertShader;
    std::unique_ptr<Shader> fragShader;
    std::unique_ptr<ShaderProgram> shaderProgram;
  };
} // namespace renderer

#endif // RENDERER_RENDERER_BASE_API_H
