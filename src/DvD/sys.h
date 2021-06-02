#ifndef DVD_SYS_H
#define DVD_SYS_H
#ifndef COMPILER

#include "globals.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <math.h>

#include <SDL_video.h>

namespace sys {
	/// @brief Create GL context
	void init();
	/// @brief Delete GL context
	void deinit();

	void setTitle(const char* title);

	/// @brief Update one frame and draw
	void refresh();

	/// @brief Get string from clipboard
	std::string getClipboard();

	void toggleFullscreen();

	/// @brief Wait
	/// @param time ms to wait
	void sleep(unsigned int time);

	/// @brief Get elasped time
	/// @return Time in ms.
	unsigned long getTime();

	extern unsigned int frame;

	extern SDL_Window* window;
}

#endif // COMPILER
#endif // DVD_SYS_H
