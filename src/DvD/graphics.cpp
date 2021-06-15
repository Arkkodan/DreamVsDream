#include "graphics.h"

#include "../renderer/renderer.h"
#include "../util/fileIO.h"
#include "error.h"
#include "input.h"
#include "shader_renderer/fighter_renderer.h"
#include "shader_renderer/primitive_renderer.h"
#include "shader_renderer/texture2D_renderer.h"
#include "sys.h"
#ifdef GAME
#include "../fileIO/text.h"
#include "../renderer/shader.h"
#include "../util/rng.h"
#include "network.h"
#include "scene/fight.h"
#include "scene/options.h"
#include "scene/scene.h"
#include "stage.h"

#include <memory>
#endif // GAME

#include <glad/glad.h>

// Macro option to use high-res timer
// Undef to use original 16 mspf (62.5 fps)
#define EXACT_60_FPS

#define SHOW_FPS
#ifdef SHOW_FPS
#include <sstream>
#endif // SHOW_FPS

namespace graphics {
  constexpr auto FPS_BUFFER = 2;

  // State options
  Image::Render render = Image::Render::NORMAL;

  int srcX = 0;
  int srcY = 0;
  int srcW = 0;
  int srcH = 0;

  float xscale = 1.0f;
  float yscale = 1.0f;

  // Immutable stuff
  unsigned int max_texture_size = 0;
  bool force_POT = false;
  bool shader_support = false;

#ifdef EXACT_60_FPS
#define timer_t uint64_t
#else
#define timer_t unsigned long
#endif // EXACT_60_FPS
  // Timer stuff
  timer_t time = 0;

#ifdef SHOW_FPS
  constexpr auto FPS_COUNTER_SIZE = 64;
  timer_t tickValues[FPS_COUNTER_SIZE] = {0};
  timer_t tickSum = 0;
  unsigned int tickIndex = 0;
#endif // SHOW_FPS
#undef timer_t

#ifdef GAME
  static std::unique_ptr<renderer::ShaderProgram> shader_palette;
#endif

  void init(bool disable_shaders_, unsigned int max_texture_size_) {
    if (!renderer::init()) {
      error::die("Could not initialize OpenGL");
    }

    renderer::FighterRenderer::init();
    renderer::PrimitiveRenderer::init();
    renderer::Texture2DRenderer::init();

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, sys::WINDOW_WIDTH, sys::WINDOW_HEIGHT, 0, -1, 1); // -1.0f, 1.0f?
    glMatrixMode(GL_MODELVIEW);

    // OpenGL's matrices are column-major
    float u_mvp[16] = {2.0f / sys::WINDOW_WIDTH,
                       0.0f,
                       0.0f,
                       0.0f,

                       0.0f,
                       -2.0f / sys::WINDOW_HEIGHT,
                       0.0f,
                       0.0f,

                       0.0f,
                       0.0f,
                       1.0f,
                       0.0f,

                       -1.0f,
                       1.0f,
                       0.0f,
                       1.0f};

    renderer::FighterRenderer::setMVPMatrix(u_mvp);
    renderer::PrimitiveRenderer::setMVPMatrix(u_mvp);
    renderer::Texture2DRenderer::setMVPMatrix(u_mvp);

    // Get max texture size
    glGetIntegerv(GL_MAX_TEXTURE_SIZE, (GLint *)&max_texture_size);
    if (max_texture_size_ && max_texture_size_ < max_texture_size) {
      max_texture_size = max_texture_size_;
    }

#ifdef SPRTOOL
    (void)disable_shaders_;
#else // !SPRTOOL
#ifdef __APPLE__
    // These functions are all available in OS X 10.6+
    shader_support = !disable_shaders_;
#else // !__APPLE__
    // Shader support?
    if (!disable_shaders_) {
      // See if we have opengl >= 2.0; this means we have non-arb
      // shading extensions
      GLint version;
      glGetIntegerv(GL_MAJOR_VERSION, &version);
      if (version >= 2) {
        shader_support =
            glAttachShader && glCompileShader && glCreateProgram &&
            glDeleteProgram && glCreateShader && glDeleteShader &&
            glGetProgramInfoLog && glGetProgramiv && glGetShaderInfoLog &&
            glGetShaderiv && glGetUniformLocation && glLinkProgram &&
            glShaderSource && glUniform1i && glUniform1f && glUniform2f &&
            glUniform3f && glUniform4f && glUseProgram;
      }
    }
    // Load shaders, if possible
    if (shader_support) {
      renderer::Shader vertex(GL_VERTEX_SHADER, fileIO::readText(util::getPath(
                                                    "shaders/vertex.v.glsl")));
      renderer::Shader fragment(
          GL_FRAGMENT_SHADER,
          fileIO::readText(util::getPath("shaders/palette.f.glsl")));
      shader_palette =
          std::make_unique<renderer::ShaderProgram>(vertex, fragment);
    }

    renderer::ShaderProgram::unuse();

#endif // !__APPLE__
#endif // !SPRTOOL
  }

  void deinit() {
#ifndef SPRTOOL
    // delete [] blankTex;
#endif
  }

  int pixel = 2;
  int shift = 0;

  void refresh() {
#ifdef GAME
    if (Stage::stage == 3) {
      // Update pixel value
      if (pixel > 2) {
        pixel--;
      }

      if (FIGHT->round >= 1) {
        if (!util::roll(64)) {
          pixel = 2 + 2 * FIGHT->round;
        }
      }
      shift = 0;

      if (!scene::Options::optionEpilepsy) {
        if (FIGHT->round >= 2) {
          if (!util::roll(64)) {
            shift = util::roll(1, 2);
          }
        }
      }
    }
#endif

    // Calculate fps, wait
#ifdef EXACT_60_FPS
#define signed_t int64_t
#define getTimeFunc() sys::getHiResTime()
    uint64_t counter = getTimeFunc();
    uint64_t frequency = sys::getHiResFrequency();
    uint64_t delta = counter - time;
#else
#define signed_t long
#define getTimeFunc() sys::getTime()
    unsigned long counter = getTimeFunc();
    unsigned long frequency = 1000; // Resolution of getTime and sleep is 1000
    unsigned long delta = counter - time;
#endif // EXACT_60_FPS
    if (static_cast<signed_t>(delta - frequency / sys::FPS) < 0) {
      if (static_cast<signed_t>(delta - frequency * FPS_BUFFER / sys::FPS) >
          0) {
        sys::sleep((static_cast<signed_t>(frequency / sys::FPS - delta) * 1000 /
                    frequency));
      }
      for (;;) {
        counter = getTimeFunc();
        if (static_cast<signed_t>(counter - time - frequency / sys::FPS) >= 0) {
          break;
        }
      }
    }
#undef getTimeFunc
#undef signed_t

#ifdef SHOW_FPS
    delta = counter - time;
    tickSum -= tickValues[tickIndex];
    tickSum += delta;
    tickValues[tickIndex] = delta;
    if (++tickIndex >= FPS_COUNTER_SIZE) {
      tickIndex = 0;
    }
    std::stringstream ss;
    ss << '['
       << std::to_string(static_cast<double>(frequency) * FPS_COUNTER_SIZE /
                         tickSum)
       << "] " << sys::WINDOW_TITLE;
    sys::setTitle(ss.str());
#endif // SHOW_FPS

    time = counter;

    glClear(GL_COLOR_BUFFER_BIT);
  }

  // State stuff
  void setClearColor(uint8_t r, uint8_t g, uint8_t b) {
    glClearColor(r / 255.0f, g / 255.0f, b / 255.0f, 1.0f);
  }

  void setColor(uint8_t r, uint8_t g, uint8_t b, float a) {
    glColor4f(r / 255.0f, g / 255.0f, b / 255.0f, a);
  }

  void setRect(int sX, int sY, int sW, int sH) {
    srcX = sX;
    srcY = sY;
    srcW = sW;
    srcH = sH;
  }

  void setScale(float scale_, float yscale_) {
    xscale = scale_;
    if (yscale_) {
      yscale = yscale_;
    }
    else {
      yscale = scale_;
    }
  }

  void setRender(Image::Render render_) { render = render_; }

#ifdef GAME
  void setPalette(unsigned int palette, float alpha, float r, float g, float b,
                  float pct) {
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, palette);
    glActiveTexture(GL_TEXTURE0);

    shader_palette->use();

    int u_texture[1] = {0};
    shader_palette->setUniform1iv("texture", u_texture);
    int u_palette[1] = {1};
    shader_palette->setUniform1iv("palette", u_palette);

    float u_shift[1] = {0.0f};
    if (Stage::stage == 3 && FIGHT->round >= 2) {
      u_shift[0] = shift / 256.0f;
    }
    shader_palette->setUniform1fv("shift", u_shift);

    float u_color[3] = {r, g, b};
    shader_palette->setUniform3fv("color", u_color);
    float u_pct[1] = {pct};
    shader_palette->setUniform1fv("pct", u_pct);

    float u_alpha[1] = {alpha};
    shader_palette->setUniform1fv("alpha", u_alpha);

    int u_pixel[1] = {1};
    if (Stage::stage == 3) {
      u_pixel[0] = pixel;
    }
    shader_palette->setUniform1iv("pixel", u_pixel);
  }
#endif
} // namespace graphics
