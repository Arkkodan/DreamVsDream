#include "texture2D_renderer.h"

#include "../../fileIO/text.h"
#include "../../util/fileIO.h"
#include "../error.h"

#include <glm/gtc/type_ptr.hpp>

#include <array>

renderer::Texture2DRenderer::Texture2DRenderer()
    : x1(0), x2(0), y1(0), y2(0), s1(0), s2(0), t1(0), t2(0) {}

renderer::Texture2DRenderer::~Texture2DRenderer() {}

std::unique_ptr<renderer::Texture2DRenderer> renderer::Texture2DRenderer::s_;

void renderer::Texture2DRenderer::init() {
  s_ = std::make_unique<Texture2DRenderer>();

  std::string vertSource =
      fileIO::readText(util::getPath("shaders/texture2D.vert.glsl"));
  std::string fragSource =
      fileIO::readText(util::getPath("shaders/texture2D.frag.glsl"));

  s_->vertShader = std::make_unique<Shader>(GL_VERTEX_SHADER, vertSource);
  s_->fragShader = std::make_unique<Shader>(GL_FRAGMENT_SHADER, fragSource);
  s_->shaderProgram =
      std::make_unique<ShaderProgram>(*(s_->vertShader), *(s_->fragShader));

  s_->shaderProgram->use();
  s_->shaderProgram->setVAP(VS_POS_LOC, Vertex, pos, GL_FLOAT, GL_FALSE);
  s_->shaderProgram->setVAP(VS_TEXCOORD_LOC, Vertex, texCoord, GL_FLOAT,
                            GL_FALSE);

  resetColor();
}

void renderer::Texture2DRenderer::setPosRect(GLfloat x1, GLfloat x2, GLfloat y1,
                                             GLfloat y2) {
  s_->x1 = x1;
  s_->x2 = x2;
  s_->y1 = y1;
  s_->y2 = y2;
}

void renderer::Texture2DRenderer::setTexRect(GLfloat s1, GLfloat s2, GLfloat t1,
                                             GLfloat t2) {
  s_->s1 = s1;
  s_->s2 = s2;
  s_->t1 = t1;
  s_->t2 = t2;
}

void renderer::Texture2DRenderer::setMVPMatrix(const glm::mat4x4 &mvp) {
  s_->shaderProgram->use();
  s_->shaderProgram->setUniformMatrix4fv(U_MVP, glm::value_ptr(mvp));
}

void renderer::Texture2DRenderer::setColor(const glm::vec4 &color) {
  s_->shaderProgram->use();
  s_->shaderProgram->setUniform4fv(U_COLOR, glm::value_ptr(color));
}

void renderer::Texture2DRenderer::setTexture2D(const Texture2D &texture,
                                               unsigned int textureOffset) {
  s_->shaderProgram->use();
  texture.activate(textureOffset);
  const int u_textureOffset[1] = {static_cast<int>(textureOffset)};
  s_->shaderProgram->setUniform1iv(U_TEXTURE2D, u_textureOffset);
}

void renderer::Texture2DRenderer::draw() {
  s_->shaderProgram->use();
  std::array<Vertex, 4> vertices = {
      glm::vec2(s_->x2, s_->y1), glm::vec2(s_->s2, s_->t1),
      glm::vec2(s_->x2, s_->y2), glm::vec2(s_->s2, s_->t2),
      glm::vec2(s_->x1, s_->y1), glm::vec2(s_->s1, s_->t1),
      glm::vec2(s_->x1, s_->y2), glm::vec2(s_->s1, s_->t2)};

  s_->vertexArray->bind();
  s_->vertexBuffer->bindData(sizeof(vertices), vertices.data(),
                             GL_DYNAMIC_DRAW);
  s_->indexBuffer->bindQuadData(1, GL_STATIC_DRAW, GL_UNSIGNED_INT);
  s_->indexBuffer->renderQuad();
}

void renderer::Texture2DRenderer::resetColor() {
  setColor({1.0f, 1.0f, 1.0f, 1.0f});
}
