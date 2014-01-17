#include "error.h"
#include <stdlib.h>
#include <iostream>
#include <string>
#include "os.h"
#include "types.h"
#include "util.h"

#ifdef EMSCRIPTEN
#include <emscripten/emscripten.h>

void alert(std::string message)
{
#if 0
	int func = EM_ASM_INT({return Runtime.addFunction(function _c_alert(message){alert(Pointer_stringify(message));});}, 0);
	void (*pointer)(const char*) = reinterpret_cast<void (*)(const char*)>(func);
	pointer(message.c_str());
	EM_ASM_INT({Runtime.removeFunction($0); return 0;}, func);
#endif
}
#endif

#ifdef _WIN32
void win_messageBox(std::string title, std::string message, int flags)
{
	wchar_t* title16 = utf8to16(title);
	wchar_t* message16 = utf8to16(message);
	
	MessageBoxW(NULL, title16, message16, flags);
	
	free(title16);
	free(message16);
}
#endif

void error(std::string sz)
{
#ifdef _WIN32
	win_messageBox("Warning", sz, MB_ICONWARNING);
#elif defined EMSCRIPTEN
	alert("Warning:\n" + sz);
#else
	std::cerr << "error: " << sz << std::endl;
#endif
}

void die(std::string sz)
{
#ifdef OS_WINDOWS
    win_messageBox("Error", sz, MB_ICONERROR);
    alert("Error:\n" + sz);
#else
	std::cerr << "fatal error: " << sz << std::endl;
#endif
    exit(1);
}

#ifdef DEBUG
void debug(std::string sz)
{
	std::cerr << "DEBUG: " << sz << std::endl;
}
#endif
