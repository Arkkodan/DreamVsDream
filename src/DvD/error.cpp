#include "error.h"

#include <stdlib.h>
#include <iostream>
#include <string>

#include "os.h"
#include "util.h"

#if !defined COMPILER && (defined _WIN32 || defined __APPLE__)
namespace os {
    extern SDL_Window* window;
}
#endif

#if defined _WIN32 && !defined COMPILER
#include <SDL_syswm.h>

namespace error {
	void w32_messageBox(const char* title, const char* text, int flags);
}

void error::w32_messageBox(const char* title, const char* text, int flags) {
	SDL_SysWMinfo info;
	SDL_VERSION(&info.version);
	if(SDL_GetWindowWMInfo(os::window, &info)) {
		wchar_t* title16 = util::utf8to16(title);
		wchar_t* text16 = util::utf8to16(text);
		MessageBoxW(info.info.win.window, text16, title16, flags);
		free(text16);
		free(title16);
	}
}
#endif

void error::error(const std::string& sz) {
#if defined __linux__ || defined COMPILER
	std::cerr << "error: " << sz << std::endl;
#else
#if defined _WIN32
	w32_messageBox("Warning", sz.c_str(), MB_ICONWARNING);
#elif defined __APPLE__
	SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_WARNING, "Warning", sz.c_str(), os::window);
#else
#error "don't know what error to use!"
#endif
#endif
}

void error::die(const std::string& sz) {
#if defined __linux__ || defined COMPILER
	std::cerr << "fatal error: " << sz << std::endl;
#else
#if defined _WIN32
	w32_messageBox("Error", sz.c_str(), MB_ICONERROR);
#elif defined __APPLE__
	SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Error", sz.c_str(), os::window);
#else
#error "don't know what error to use!"
#endif
#endif
	exit(1);
}

#ifdef DEBUG
void error::debug(const std::string& sz) {
	std::cerr << "DEBUG: " << sz << std::endl;
}
#endif
