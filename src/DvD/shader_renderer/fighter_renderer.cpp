#include "fighter_renderer.h"

#include "../../fileIO/text.h"
#include "../../util/fileIO.h"
#include "../error.h"

#include <array>

renderer::FighterRenderer::FighterRenderer()
    : x1(0), x2(0), y1(0), y2(0), s1(0), s2(0), t1(0), t2(0) {}

renderer::FighterRenderer::~FighterRenderer() {}

std::unique_ptr<renderer::FighterRenderer> renderer::FighterRenderer::s_;

void renderer::FighterRenderer::init() {
  s_ = std::make_unique<FighterRenderer>();

  std::string vertSource =
      fileIO::readText(util::getPath("shaders/texture2D.vert.glsl"));
  std::string fragSource =
      fileIO::readText(util::getPath("shaders/palette330.frag.glsl"));

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

void renderer::FighterRenderer::setPosRect(GLfloat x1, GLfloat x2, GLfloat y1,
                                           GLfloat y2) {
  s_->x1 = x1;
  s_->x2 = x2;
  s_->y1 = y1;
  s_->y2 = y2;
}

void renderer::FighterRenderer::setTexRect(GLfloat s1, GLfloat s2, GLfloat t1,
                                           GLfloat t2) {
  s_->s1 = s1;
  s_->s2 = s2;
  s_->t1 = t1;
  s_->t2 = t2;
}

void renderer::FighterRenderer::setMVPMatrix(GLfloat *mvp) {
  s_->shaderProgram->use();
  s_->shaderProgram->setUniformMatrix4fv(U_MVP, mvp);
}

void renderer::FighterRenderer::setColor(GLfloat r, GLfloat g, GLfloat b) {
  s_->shaderProgram->use();
  const float u_color[4] = {r, g, b, 1.0f}; // We do not care about alpha here
  s_->shaderProgram->setUniform4fv(U_COLOR, u_color);
}

void renderer::FighterRenderer::setTexture2D(const Texture2D &texture,
                                             unsigned int textureOffset) {
  s_->shaderProgram->use();
  texture.activate(textureOffset);
  const int u_textureOffset[1] = {static_cast<int>(textureOffset)};
  s_->shaderProgram->setUniform1iv(U_TEXTURE2D, u_textureOffset);
}

void renderer::FighterRenderer::setPalette(const Texture2D &palette,
                                           unsigned int textureOffset) {
  s_->shaderProgram->use();
  palette.activate(textureOffset);
  const int u_textureOffset[1] = {static_cast<int>(textureOffset)};
  s_->shaderProgram->setUniform1iv(U_PALETTE, u_textureOffset);
}

void renderer::FighterRenderer::setAlpha(GLfloat alpha) {
  s_->shaderProgram->use();
  const float u_alpha[1] = {alpha};
  s_->shaderProgram->setUniform1fv(U_ALPHA, u_alpha);
}

void renderer::FighterRenderer::setPercent(GLfloat pct) {
  s_->shaderProgram->use();
  const float u_pct[1] = {pct};
  s_->shaderProgram->setUniform1fv(U_PERCENT, u_pct);
}

void renderer::FighterRenderer::setShift(GLfloat shift) {
  s_->shaderProgram->use();
  const float u_shift[1] = {shift};
  s_->shaderProgram->setUniform1fv(U_SHIFT, u_shift);
}

void renderer::FighterRenderer::setPixelSize(GLint pixelSize) {
  s_->shaderProgram->use();
  const int u_pixelSize[1] = {pixelSize};
  s_->shaderProgram->setUniform1iv(U_PIXELSIZE, u_pixelSize);
}

void renderer::FighterRenderer::draw() {
  s_->shaderProgram->use();
  std::array<Vertex, 4> vertices = {s_->x2, s_->y1, s_->s2, s_->t1,

                                    s_->x2, s_->y2, s_->s2, s_->t2,

                                    s_->x1, s_->y1, s_->s1, s_->t1,

                                    s_->x1, s_->y2, s_->s1, s_->t2};

  s_->vertexArray->bind();
  s_->vertexBuffer->bindData(sizeof(vertices), vertices.data(),
                             GL_DYNAMIC_DRAW);
  s_->indexBuffer->bindQuadData(1, GL_STATIC_DRAW, GL_UNSIGNED_INT);
  s_->indexBuffer->renderQuad();
}

void renderer::FighterRenderer::resetColor() {
  setColor(1.0f, 1.0f, 1.0f);
  setAlpha(1.0f);
}
