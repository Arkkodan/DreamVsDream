#include "error.h"
#include <stdlib.h>
#include <iostream>
#include <string>
#include "os.h"
#include "util.h"

#ifdef EMSCRIPTEN
#include <emscripten/emscripten.h>

void alert(std::string message) {
}
#endif

#ifdef _WIN32
void win_messageBox(std::string title, std::string message, int flags) {
	wchar_t* title16 = util::utf8to16(title.c_str());
	wchar_t* message16 = util::utf8to16(message.c_str());

	MessageBoxW(NULL, title16, message16, flags);

	free(title16);
	free(message16);
}
#endif

void error(std::string sz) {
#ifdef _WIN32
	win_messageBox("Warning", sz, MB_ICONWARNING);
#elif defined EMSCRIPTEN
	alert("Warning:\n" + sz);
#else
	std::cerr << "error: " << sz << std::endl;
#endif
}

void die(std::string sz) {
#ifdef _WIN32
	win_messageBox("Error", sz, MB_ICONERROR);
#elif defined EMSCRIPTEN
	alert("Error:\n" + sz);
#else
	std::cerr << "fatal error: " << sz << std::endl;
#endif
	exit(1);
}

#ifdef DEBUG
void debug(std::string sz) {
	std::cerr << "DEBUG: " << sz << std::endl;
}
#endif
