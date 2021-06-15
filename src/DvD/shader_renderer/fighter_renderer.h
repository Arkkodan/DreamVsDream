#ifndef DVD_SHADER_RENDERER_FIGHTER_RENDERER_H
#define DVD_SHADER_RENDERER_FIGHTER_RENDERER_H

#include "../../renderer/renderer_base_api.h"

#include "../../renderer/texture2D.h"

namespace renderer {
  class FighterRenderer : public Renderer {
  private:
    // Singleton access (dynamic_cast Renderer *)
    static std::unique_ptr<FighterRenderer> s_;

  private:
    struct Vertex {
      GLfloat pos[2];
      GLfloat texCoord[2];
    };

  public:
    FighterRenderer();
    virtual ~FighterRenderer() override final;

  public:
    static void init();

    // Inputs to vertex shader
    static void setPosRect(GLfloat x1, GLfloat x2, GLfloat y1, GLfloat y2);
    static void setTexRect(GLfloat s1, GLfloat s2, GLfloat t1, GLfloat t2);

    // Uniforms for vertex shader
    static void setMVPMatrix(GLfloat *mvp);
    static void setColor(GLfloat r, GLfloat g, GLfloat b);

    // Uniforms for fragment shader
    static void setTexture2D(const Texture2D &texture,
                             unsigned int textureOffset);
    static void setPalette(const Texture2D &palette,
                           unsigned int textureOffset);
    static void setAlpha(GLfloat alpha);
    static void setPercent(GLfloat pct);
    static void setShift(GLfloat shift);
    static void setPixelSize(GLint pixelSize);

    static void draw();

  private:
    GLfloat x1, x2, y1, y2;
    GLfloat s1, s2, t1, t2;

    static constexpr auto VS_POS_LOC = 0;
    static constexpr auto VS_TEXCOORD_LOC = 1;
    static constexpr auto U_MVP = "u_mvp";
    static constexpr auto U_COLOR = "u_color";
    static constexpr auto U_TEXTURE2D = "u_texture2D";
    static constexpr auto U_PALETTE = "u_palette";
    static constexpr auto U_ALPHA = "u_alpha";
    static constexpr auto U_PERCENT = "u_pct";
    static constexpr auto U_SHIFT = "u_shift";
    static constexpr auto U_PIXELSIZE = "u_pixel";
  };
} // namespace renderer

#endif // DVD_SHADER_RENDERER_FIGHTER_RENDERER_H
