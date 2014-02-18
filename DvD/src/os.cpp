#include "os.h"
#include "error.h"
#include "globals.h"
#include "network.h"
#include "input.h"
#include "graphics.h"

extern game::Player madotsuki;
extern game::Player poniko;

namespace os {
	unsigned int frame = 0;

	SDL_Window* window = nullptr;
	SDL_GLContext glcontext = nullptr;

	void init() {
		//Initialize SDL
		if(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0) {
			die("Failed to initialize SDL.");
		}

		//Create Window
		if(!(window = SDL_CreateWindow(WINDOW_TITLE, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_OPENGL))) {
			die("Failed to initialize SDL.");
		}

        //Create the opengl context
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
		glcontext = SDL_GL_CreateContext(window);
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
		madotsuki.frameInput = 0;
		poniko.frameInput = 0;
#endif

		SDL_GL_SwapWindow(window);

		SDL_Event e;
		while(SDL_PollEvent(&e)) {
			if(e.type == SDL_QUIT) {
				exit(0);
			} else if(e.type == SDL_KEYDOWN) {
				if(e.key.keysym.sym == SDLK_ESCAPE) {
					exit(0);
				} else
					input::keyPress(e.key.keysym.sym, true);
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
