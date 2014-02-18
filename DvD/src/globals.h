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

#ifdef SPRTOOL
#define WINDOW_TITLE "Dream vs. Dream Sprite Tool"
#else
#define WINDOW_TITLE "Dream vs. Dream v0.2>"
#endif
#define WINDOW_WIDTH 640
#define WINDOW_HEIGHT 480

#define FLIP(x) ((WINDOW_HEIGHT - 1) - (x))

#define FPS (60)
#define SPF (1.0/FPS)
#define MSPF (1000/FPS)

#define FLOAT_ACCURACY 65536

extern std::string szConfigPath;

extern int optionDifficulty;
extern int optionWins;
extern int optionTime;
extern int optionSfxVolume;
extern int optionMusVolume;
extern int optionVoiceVolume;
extern bool optionEpilepsy;
extern bool optionSecretCharacter;

//SPRITE TOOL STUFF
#ifdef SPRTOOL
#define EDIT_OFFSET 100
#endif

#endif // GLOBALS_H_INCLUDED
