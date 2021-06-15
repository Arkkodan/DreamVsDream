#ifndef DVD_SHADER_RENDERER_PRIMITIVE_RENDERER_H
#define DVD_SHADER_RENDERER_PRIMITIVE_RENDERER_H

#include "../../renderer/renderer_base_api.h"

namespace renderer {
  class PrimitiveRenderer : public Renderer {
  private:
    // Singleton access (dynamic_cast Renderer *)
    static std::unique_ptr<PrimitiveRenderer> s_;

  private:
    struct Vertex {
      GLfloat pos[2];
    };

  public:
    PrimitiveRenderer();
    virtual ~PrimitiveRenderer() override final;

  public:
    static void init();

    // Inputs to vertex shader
    static void setPosRect(GLfloat x1, GLfloat x2, GLfloat y1, GLfloat y2);

    // Uniforms for vertex shader
    static void setMVPMatrix(GLfloat *mvp);
    static void setColor(GLfloat r, GLfloat g, GLfloat b, GLfloat a);

    static void draw();

  private:
    GLfloat x1, x2, y1, y2;

    static constexpr auto VS_POS_LOC = 0;
    static constexpr auto U_MVP = "u_mvp";
    static constexpr auto U_COLOR = "u_color";
  };
} // namespace renderer

#endif // DVD_SHADER_RENDERER_PRIMITIVE_RENDERER_H
