#include "sys.h"

#include "error.h"
#include "network.h"
#include "input.h"
#include "graphics.h"

#include "scene/fight.h"

#include <SDL.h>
#include <glad/glad.h>

namespace sys {
	unsigned int frame = 0;

	SDL_Window* window = nullptr;
	SDL_GLContext glcontext = nullptr;

	void init() {
		//Initialize SDL
		if(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0) {
			error::die("Failed to initialize SDL.");
		}

		//Create Window
		if(!(window = SDL_CreateWindow(WINDOW_TITLE, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_OPENGL))) {
			error::die("Failed to initialize SDL.");
		}

        //Create the opengl context
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

	void sleep(unsigned int time) {
		SDL_Delay(time);
	}

	unsigned long getTime() {
		return SDL_GetTicks();
	}

	void refresh() {
#ifdef GAME
		SceneFight::madotsuki.frameInput = 0;
		SceneFight::poniko.frameInput = 0;
#endif

		SDL_GL_SwapWindow(window);

		SDL_Event e;
		while(SDL_PollEvent(&e)) {
			if(e.type == SDL_QUIT) {
				exit(0);
			} else if(e.type == SDL_KEYDOWN) {
				if(e.key.keysym.sym == SDLK_ESCAPE) {
					exit(0);
				} else if(!e.key.repeat) {
					input::keyPress(e.key.keysym.sym, true);
				}
			} else if(e.type == SDL_KEYUP) {
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
	    if(SDL_HasClipboardText())
            return std::string(SDL_GetClipboardText());
		return "";
	}
}
