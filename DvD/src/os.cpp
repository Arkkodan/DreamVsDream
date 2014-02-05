#include "os.h"
#include "error.h"
#include "globals.h"
#include "network.h"
#include "input.h"
#include "graphics.h"

#ifdef EMSCRIPTEN
#include <SDL_image.h>
#endif

extern game::Player madotsuki;
extern game::Player poniko;

namespace os {
	unsigned int frame = 0;

	void init() {
		//Initialize SDL
		if(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0) {
			die("Failed to initialize SDL.");
		}

		//Create Window
		SDL_WM_SetCaption(WINDOW_TITLE, NULL);
		if(SDL_SetVideoMode(WINDOW_WIDTH, WINDOW_HEIGHT, 24, SDL_OPENGL) == NULL) {
			die("Failed to initialize SDL.");
		}
#ifdef EMSCRIPTEN
		IMG_Init(IMG_INIT_PNG);
#endif
	}

	void deinit() {
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

		SDL_GL_SwapBuffers();

		SDL_Event e;
		while(SDL_PollEvent(&e)) {
			if(e.type == SDL_QUIT) {
				exit(0);
			} else if(e.type == SDL_KEYDOWN) {
#ifndef EMSCRIPTEN
				if(e.key.keysym.sym == SDLK_ESCAPE) {
					exit(0);
				} else
#endif
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

	void getClipboard(char* sz, size_t size) {
		*sz = 0;
	}
}
