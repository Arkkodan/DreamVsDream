#ifndef OS_H_INCLUDED
#define OS_H_INCLUDED

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <math.h>

#include <GL/glew.h>
#define NO_SDL_GLEXT
#include <SDL/SDL.h>
#include <SDL/SDL_opengl.h>

#include "types.h"

namespace os {
	void init();
	void deinit();

	void setTitle(const char* title);
	void refresh();

	void getClipboard(char* b_sz, size_t size);

	void toggleFullscreen();

	void sleep(unsigned int time);

	unsigned long getTime();

	extern unsigned int gameFrame;
}

#endif // OS_H_INCLUDED
