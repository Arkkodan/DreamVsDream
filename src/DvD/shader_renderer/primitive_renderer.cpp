#include "primitive_renderer.h"

#include "../../fileIO/text.h"
#include "../../util/fileIO.h"
#include "../error.h"

#include <array>

renderer::PrimitiveRenderer::PrimitiveRenderer() : x1(0), x2(0), y1(0), y2(0) {}

renderer::PrimitiveRenderer::~PrimitiveRenderer() {}

std::unique_ptr<renderer::PrimitiveRenderer> renderer::PrimitiveRenderer::s_;

void renderer::PrimitiveRenderer::init() {
  s_ = std::make_unique<PrimitiveRenderer>();

  std::string vertSource =
      fileIO::readText(util::getPath("shaders/primitive.vert.glsl"));
  std::string fragSource =
      fileIO::readText(util::getPath("shaders/primitive.frag.glsl"));

  s_->vertShader = std::make_unique<Shader>(GL_VERTEX_SHADER, vertSource);
  s_->fragShader = std::make_unique<Shader>(GL_FRAGMENT_SHADER, fragSource);
  s_->shaderProgram =
      std::make_unique<ShaderProgram>(*(s_->vertShader), *(s_->fragShader));

  s_->shaderProgram->use();
  s_->shaderProgram->setVAP(VS_POS_LOC, Vertex, pos, GL_FLOAT, GL_FALSE);
}

void renderer::PrimitiveRenderer::setPosRect(GLfloat x1, GLfloat x2, GLfloat y1,
                                             GLfloat y2) {
  s_->x1 = x1;
  s_->x2 = x2;
  s_->y1 = y1;
  s_->y2 = y2;
}

void renderer::PrimitiveRenderer::setMVPMatrix(GLfloat *mvp) {
  s_->shaderProgram->use();
  s_->shaderProgram->setUniformMatrix4fv(U_MVP, mvp);
}

void renderer::PrimitiveRenderer::setColor(GLfloat r, GLfloat g, GLfloat b,
                                           GLfloat a) {
  s_->shaderProgram->use();
  const float u_color[4] = {r, g, b, a};
  s_->shaderProgram->setUniform4fv(U_COLOR, u_color);
}

void renderer::PrimitiveRenderer::draw() {
  s_->shaderProgram->use();
  std::array<Vertex, 4> vertices = {
      s_->x2, s_->y1,

      s_->x2, s_->y2,

      s_->x1, s_->y1,

      s_->x1, s_->y2,
  };

  s_->vertexArray->bind();
  s_->vertexBuffer->bindData(sizeof(vertices), vertices.data(),
                             GL_DYNAMIC_DRAW);
  s_->indexBuffer->bindQuadData(1, GL_STATIC_DRAW, GL_UNSIGNED_INT);
  s_->indexBuffer->renderQuad();
}
