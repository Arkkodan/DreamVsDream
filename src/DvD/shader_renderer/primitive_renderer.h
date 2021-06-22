#ifndef DVD_SHADER_RENDERER_PRIMITIVE_RENDERER_H
#define DVD_SHADER_RENDERER_PRIMITIVE_RENDERER_H

#include "../../renderer/renderer_base_api.h"

#include <glm/mat4x4.hpp>
#include <glm/vec2.hpp>
#include <glm/vec4.hpp>

namespace renderer {
  class PrimitiveRenderer : public Renderer {
  private:
    // Singleton access (dynamic_cast Renderer *)
    static std::unique_ptr<PrimitiveRenderer> s_;

  private:
    struct Vertex {
      glm::vec2 pos;
    };

  public:
    PrimitiveRenderer();
    virtual ~PrimitiveRenderer() override final;

  public:
    static void init();

    // Inputs to vertex shader
    static void setPosRect(GLfloat x1, GLfloat x2, GLfloat y1, GLfloat y2);

    // Uniforms for vertex shader
    static void setMVPMatrix(const glm::mat4x4 &mvp);
    static void setColor(const glm::vec4 &color);

    static void draw();

  private:
    GLfloat x1, x2, y1, y2;

    static constexpr auto VS_POS_LOC = 0;
    static constexpr auto U_MVP = "u_mvp";
    static constexpr auto U_COLOR = "u_color";
  };
} // namespace renderer

#endif // DVD_SHADER_RENDERER_PRIMITIVE_RENDERER_H
