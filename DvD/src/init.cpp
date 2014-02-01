#include <stdlib.h>
#include <time.h>
#include <stdio.h>
#ifndef _WIN32
#include <sys/stat.h>
#endif

#include "fighter.h"
#include "graphics.h"
#include "error.h"
#include "menu.h"
#include "sound.h"
#include "stage.h"
#include "network.h"
#include "file.h"
#include "input.h"

std::string szConfigPath;

static bool disable_shaders = false;
static bool disable_sound = false;
static int max_texture_size = 0;
static bool versus = false;
static bool fullscreen = false;
static int input_delay = 0;

void parseArgs(int argc, char** argv) {
	extern fighter::Player madotsuki;
	extern fighter::Player poniko;
	extern int madotsuki_palette;
	extern int poniko_palette;

	madotsuki.fighter = &fighter::fighters[0];
	poniko.fighter = &fighter::fighters[0];

	for(int i = 1; i < argc; i++) {
		if(!strcasecmp(argv[i], "--disable-shaders")) {
			disable_shaders = true;
		} else if(!strcasecmp(argv[i], "--disable-sound")) {
			disable_sound = true;
		} else if(!strcasecmp(argv[i], "--training")) {
			menu = MENU_FIGHT;
		} else if(!strcasecmp(argv[i], "--versus")) {
			menu = MENU_FIGHT;
			versus = true;
		} else if(!strcasecmp(argv[i], "--fullscreen")) {
			fullscreen = true;
		} else if(!strcasecmp(argv[i], "-char1")) {
			if(++i < argc) {
				madotsuki.fighter = &fighter::fighters[atoi(argv[i])];
			}
		} else if(!strcasecmp(argv[i], "-char2")) {
			if(++i < argc) {
				poniko.fighter = &fighter::fighters[atoi(argv[i])];
			}
		} else if(!strcasecmp(argv[i], "-pal1")) {
			if(++i < argc) {
				madotsuki_palette = atoi(argv[i]);
			}
		} else if(!strcasecmp(argv[i], "-pal2")) {
			if(++i < argc) {
				poniko_palette = atoi(argv[i]);
			}
		} else if(!strcasecmp(argv[i], "-stage")) {
			if(++i < argc) {
				stage = atoi(argv[i]);
			}
		} else if(!strcasecmp(argv[i], "-input-delay")) {
			if(++i < argc) {
				input_delay = atoi(argv[i]);
			}
		} else if(!strcasecmp(argv[i], "-max-texture-size")) {
			if(++i < argc) {
				max_texture_size = atoi(argv[i]);
			}
		}
	}
}

#ifndef EMSCRIPTEN
void optionsLoad() {
	File file;
	if(!file.open(FILE_READ_NORMAL, szConfigPath + "options.dat")) {
		return;
	}

	optionDifficulty = file.readByte();
	optionWins = file.readByte();
	optionTime = file.readByte();
	optionSfxVolume = file.readByte();
	optionMusVolume = file.readByte();
	optionVoiceVolume = file.readByte();
	optionEpilepsy = file.readByte();
	optionSecretCharacter = file.readByte();

	file.close();
}

void optionsSave() {
	File file;
	if(!file.open(FILE_WRITE_NORMAL, szConfigPath + "options.dat")) {
		return;
	}

	file.writeByte(optionDifficulty);
	file.writeByte(optionWins);
	file.writeByte(optionTime);
	file.writeByte(optionSfxVolume);
	file.writeByte(optionMusVolume);
	file.writeByte(optionVoiceVolume);
	file.writeByte(optionEpilepsy);
	file.writeByte(optionSecretCharacter);

	file.close();
}
#endif

void init() {
	void deinit();
	atexit(deinit);

	srand(time(NULL));

#ifndef EMSCRIPTEN
	//Create settings path first
	//Find correct directory
#if defined _WIN32
#define CONFIG_PATH "\\dreamvsdream\\"
	wchar_t* env16 = _wgetenv(L"AppData");
	if(!env16) {
		die("Cannot find settings directory.");
	}
	char* env = utf16to8(env16);
#elif defined __APPLE__
#define CONFIG_PATH "/Library/Application Support/dreamvsdream/"
	char* env = getenv("HOME");
	if(!env) {
		die("Cannot find settings directory.");
	}
#else
#define CONFIG_PATH "/dreamvsdream/"
	char* env = getenv("XDG_CONFIG_HOME");
	if(!env) {
		env = getenv("HOME");
		if(!env) {
			die("Cannot find settings directory.");
		}
	}
#endif

	//Allocate into new string
	szConfigPath = std::string(env) + CONFIG_PATH;

	//Create directory if it doesn't exist
#ifdef _WIN32
	free(env);
	wchar_t* szConfigPath16 = utf8to16(szConfigPath.c_str());
	CreateDirectoryW(szConfigPath16, NULL);
	free(szConfigPath16);
#else
	mkdir(szConfigPath.c_str(), 0777);
#endif

	optionsLoad();
#endif

	os::init();
	input::init();
	if(!disable_sound) {
		audio::init();
	}
	net::init(input_delay);
	graphics::init(disable_shaders, max_texture_size);
	fighter::init();
	Menu::ginit(); //TODO fix this
	Stage::ginit();
	sprite::init();

	if(versus) {
		FIGHT->gametype = GAMETYPE_VERSUS;
	}
	//if(fullscreen)
	//OS::toggleFullscreen();

	if(menu == MENU_FIGHT && stage == -1) {
		stage = 0;
	}
}

void deinit() {
	sprite::deinit();
	Stage::deinit();
	Menu::deinit();
	fighter::deinit();
	graphics::deinit();
	net::deinit();
	audio::deinit();

	os::deinit();

#ifndef EMSCRIPTEN
	optionsSave();
#endif
}
