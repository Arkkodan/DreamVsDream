#ifndef DVD_SHADER_RENDERER_TEXTURE2D_RENDERER_H
#define DVD_SHADER_RENDERER_TEXTURE2D_RENDERER_H

#include "../../renderer/renderer_base_api.h"

#include "../../renderer/texture2D.h"

namespace renderer {
  class Texture2DRenderer : public Renderer {
  private:
    // Singleton access (dynamic_cast Renderer *)
    static std::unique_ptr<Texture2DRenderer> s_;

  private:
    struct Vertex {
      GLfloat pos[2];
      GLfloat texCoord[2];
    };

  public:
    Texture2DRenderer();
    virtual ~Texture2DRenderer() override final;

  public:
    static void init();

    // Inputs to vertex shader
    static void setPosRect(GLfloat x1, GLfloat x2, GLfloat y1, GLfloat y2);
    static void setTexRect(GLfloat s1, GLfloat s2, GLfloat t1, GLfloat t2);

    // Uniforms for vertex shader
    static void setMVPMatrix(GLfloat *mvp);
    static void setColor(GLfloat r, GLfloat g, GLfloat b, GLfloat a);

    // Uniforms for fragment shader
    static void setTexture2D(const Texture2D &texture,
                             unsigned int textureOffset);

    static void draw();
    static void resetColor();

  private:
    GLfloat x1, x2, y1, y2;
    GLfloat s1, s2, t1, t2;

    static constexpr auto VS_POS_LOC = 0;
    static constexpr auto VS_TEXCOORD_LOC = 1;
    static constexpr auto U_MVP = "u_mvp";
    static constexpr auto U_COLOR = "u_color";
    static constexpr auto U_TEXTURE2D = "u_texture2D";
  };
} // namespace renderer

#endif // DVD_SHADER_RENDERER_TEXTURE2D_RENDERER_H
