#include "sys.h"

#include "app.h"
#include "error.h"
#include "graphics.h"
#include "input.h"
#ifdef GAME
#include "network.h"
#include "player.h"
#include "scene/fight.h"
#include "sound.h"
#endif

#include <SDL.h>
#include <glad/glad.h>

namespace sys {
  static unsigned int frame = 0;

  static SDL_Window *window = nullptr;
  static SDL_GLContext glcontext = nullptr;

  unsigned int getFrame() { return frame; }
  SDL_Window *getWindow() { return window; }

  void init() {
    // Initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0) {
      error::die("Failed to initialize SDL.");
    }

    // Create Window
    if (!(window = SDL_CreateWindow(WINDOW_TITLE, SDL_WINDOWPOS_CENTERED,
                                    SDL_WINDOWPOS_CENTERED, WINDOW_WIDTH,
                                    WINDOW_HEIGHT, SDL_WINDOW_OPENGL))) {
      error::die("Failed to initialize SDL.");
    }

    // Create the opengl context
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
    glcontext = SDL_GL_CreateContext(window);

    // Load GL extensions using glad
    if (!gladLoadGLLoader(SDL_GL_GetProcAddress)) {
      error::die("Failed to initialize the OpenGL context.");
    }
  }

  void deinit() {
    SDL_GL_DeleteContext(glcontext);
    SDL_DestroyWindow(window);
    SDL_Quit();
  }

  void setTitle(const std::string &title) {
    SDL_SetWindowTitle(window, title.c_str());
  }

  void sleep(unsigned int time) { SDL_Delay(time); }

  unsigned long getTime() { return SDL_GetTicks(); }

  uint64_t getHiResTime() { return SDL_GetPerformanceCounter(); }
  uint64_t getHiResFrequency() { return SDL_GetPerformanceFrequency(); }

  void refresh() {
#ifdef GAME
    scene::Fight::getrPlayerAt(0).setFrameInput(0);
    scene::Fight::getrPlayerAt(1).setFrameInput(0);
#endif

    SDL_GL_SwapWindow(window);

    SDL_Event e;
    while (SDL_PollEvent(&e)) {
      if (e.type == SDL_QUIT) {
#ifdef GAME
        app::quit();
#else
        exit(0);
#endif
      }
      else if (e.type == SDL_KEYDOWN) {
        if (!e.key.repeat) {
          input::keyPress(e.key.keysym.sym, true);
        }
      }
      else if (e.type == SDL_KEYUP) {
        input::keyPress(e.key.keysym.sym, false);
      }
    }

#ifdef GAME
    net::refresh();
    input::refresh();
    audio::refresh();
#endif
    graphics::refresh();
    frame++;
  }

  std::string getClipboard() {
    if (SDL_HasClipboardText())
      return std::string(SDL_GetClipboardText());
    return "";
  }
} // namespace sys
