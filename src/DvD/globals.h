#ifndef GLOBALS_H_INCLUDED
#define GLOBALS_H_INCLUDED

#include <string>
#include <vector>
#include <iostream>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdint.h>
#include <limits.h>

#ifdef _WIN32
#ifndef WINVER
#define WINVER 0x0500
#endif
#include <windows.h>
#include <shlwapi.h>
#endif

#include "os.h"

typedef char byte_t;
typedef unsigned char ubyte_t;

namespace globals {
#ifdef SPRTOOL
	constexpr auto WINDOW_TITLE = "Dream vs. Dream Sprite Tool v" VERSION;
#else
	constexpr auto WINDOW_TITLE = "Dream vs. Dream v" VERSION;
#endif
	constexpr auto WINDOW_WIDTH = 640;
	constexpr auto WINDOW_HEIGHT = 480;

	constexpr auto FLIP(int x) {
		return WINDOW_HEIGHT - 1 - x;
	}

	constexpr auto FPS = 60;
	constexpr auto SPF = 1.0 / FPS;
	constexpr auto MSPF = 1000 / FPS;

	constexpr auto FLOAT_ACCURACY = 65536;

	//SPRITE TOOL STUFF
#ifdef SPRTOOL
	constexpr auto EDIT_OFFSET = 100;
#endif
}

namespace init {
	extern std::string szConfigPath;
}

#endif // GLOBALS_H_INCLUDED
