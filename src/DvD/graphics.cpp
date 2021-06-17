#include "graphics.h"

#include "../renderer/gl_loader.h"
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

// Macro option to use high-res timer
// Undef to use original 16 mspf (62.5 fps)
#define EXACT_60_FPS

#define SHOW_FPS
#ifdef SHOW_FPS
#include <sstream>
#endif // SHOW_FPS

namespace graphics {
  static constexpr auto FPS_BUFFER = 2;

  // State options
  static Image::Render render = Image::Render::NORMAL;

  static int srcX = 0;
  static int srcY = 0;
  static int srcW = 0;
  static int srcH = 0;

  static float xscale = 1.0f;
  static float yscale = 1.0f;

  // Immutable stuff
  static unsigned int max_texture_size = 0;
  static bool shader_support = false;

  Image::Render getRender() { return render; }

  int &getrSourceX() { return srcX; }
  int &getrSourceY() { return srcY; }
  int &getrSourceW() { return srcW; }
  int &getrSourceH() { return srcH; }

  float &getrXScale() { return xscale; }
  float &getrYScale() { return yscale; }

  unsigned int getMaxTextureSize() { return max_texture_size; }
  bool hasShaderSupport() { return shader_support; }

#ifdef EXACT_60_FPS
#define timer_t uint64_t
#else
#define timer_t unsigned long
#endif // EXACT_60_FPS
  // Timer stuff
  static timer_t time = 0;

#ifdef SHOW_FPS
  static constexpr auto FPS_COUNTER_SIZE = 64;
  static timer_t tickValues[FPS_COUNTER_SIZE] = {0};
  static timer_t tickSum = 0;
  static unsigned int tickIndex = 0;
#endif // SHOW_FPS
#undef timer_t

  void init(bool disable_shaders_, unsigned int max_texture_size_) {
    if (!renderer::init()) {
      error::die("Could not initialize OpenGL");
    }

    renderer::FighterRenderer::init();
    renderer::PrimitiveRenderer::init();
    renderer::Texture2DRenderer::init();

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

    renderer::ShaderProgram::unuse();

#endif // !__APPLE__
#endif // !SPRTOOL
  }

  void deinit() {
#ifndef SPRTOOL
    // delete [] blankTex;
#endif
  }

  static int pixel = 2;
  static int shift = 0;

  void refresh() {
#ifdef GAME
    if (Stage::getStageIndex() == 3) {
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
  void setPalette(const renderer::Texture2D &palette, float alpha, float r,
                  float g, float b, float pct) {
    int stageIndex = Stage::getStageIndex();

    // u_texture set in image.cpp
    renderer::FighterRenderer::setPalette(palette, 1);

    float u_shift = 0.0f;
    if (stageIndex == 3 && FIGHT->round >= 2) {
      u_shift = shift / 256.0f;
    }
    renderer::FighterRenderer::setShift(u_shift);

    renderer::FighterRenderer::setColor(r, g, b);
    renderer::FighterRenderer::setPercent(pct);

    renderer::FighterRenderer::setAlpha(alpha);

    int u_pixel = 1;
    if (stageIndex == 3) {
      u_pixel = pixel;
    }
    renderer::FighterRenderer::setPixelSize(u_pixel);
  }
#endif
} // namespace graphics
