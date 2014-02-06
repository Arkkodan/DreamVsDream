#ifndef OS_H_INCLUDED
#define OS_H_INCLUDED

#include "globals.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <math.h>

#include <GL/glew.h>
#define NO_SDL_GLEXT
#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>

namespace os {
	void init();
	void deinit();

	void setTitle(const char* title);
	void refresh();

	std::string getClipboard();

	void toggleFullscreen();

	void sleep(unsigned int time);

	unsigned long getTime();

	extern unsigned int frame;
}

#endif // OS_H_INCLUDED
