#include "error.h"

#include <stdlib.h>
#include <iostream>
#include <string>

#include "os.h"
#include "util.h"

#ifdef EMSCRIPTEN
#include <emscripten/emscripten.h>

void alert(const std::string& sz) {
}
#endif

#if defined _WIN32 || defined __APPLE__
namespace os {
    extern SDL_Window* window;
}
#endif

void error(const std::string& sz) {
#if defined _WIN32 || defined __APPLE__
	SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_WARNING, "Warning", sz.c_str(), os::window);
#elif defined EMSCRIPTEN
	alert("Warning:\n" + sz);
#else
	std::cerr << "error: " << sz << std::endl;
#endif
}

void die(const std::string& sz) {
#if defined _WIN32 || defined __APPLE__
	SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Error", sz.c_str(), os::window);
#elif defined EMSCRIPTEN
	alert("Error:\n" + sz);
#else
	std::cerr << "fatal error: " << sz << std::endl;
#endif
	exit(1);
}

#ifdef DEBUG
void debug(const std::string& sz) {
	std::cerr << "DEBUG: " << sz << std::endl;
}
#endif
