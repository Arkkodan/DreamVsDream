#include <stdlib.h>
#include <time.h>
#include <stdio.h>
#ifndef _WIN32
#include <sys/stat.h>
#endif

#include "fighter.h"
#include "graphics.h"
#include "error.h"
#include "scene/scene.h"
#include "sound.h"
#include "stage.h"
#include "network.h"
#include "file.h"
#include "input.h"
#include "effect.h"

#if(_MSC_VER) // MSVC uses _stricmp instead of strcasecmp
#define strcasecmp(str1, str2) _stricmp(str1, str2)
#endif

namespace init {
	std::string szConfigPath;

	static bool disable_shaders = false;
	static bool disable_sound = false;
	static int max_texture_size = 0;
	static bool versus = false;
	static bool fullscreen = false;
	static int input_delay = 0;

	constexpr auto OPTIONS_VERSION = 0;

	void parseArgs(int argc, char** argv);
	void optionsLoad();
	void optionsSave();
	void init();
	void deinit();
}

namespace g_main {
	extern game::Player madotsuki;
	extern game::Player poniko;
}

void init::parseArgs(int argc, char** argv) {
	g_main::madotsuki.fighter = &game::fighters[0];
	g_main::poniko.fighter = &game::fighters[0];

	for(int i = 1; i < argc; i++) {
		if(!strcasecmp(argv[i], "--disable-shaders")) {
			disable_shaders = true;
		} else if(!strcasecmp(argv[i], "--disable-sound")) {
			disable_sound = true;
		} else if(!strcasecmp(argv[i], "--training")) {
			Scene::scene = Scene::SCENE_FIGHT;
		} else if(!strcasecmp(argv[i], "--versus")) {
			Scene::scene = Scene::SCENE_FIGHT;
			versus = true;
		} else if(!strcasecmp(argv[i], "--fullscreen")) {
			fullscreen = true;
		} else if(!strcasecmp(argv[i], "-char1")) {
			if(++i < argc) {
				g_main::madotsuki.fighter = &game::fighters[atoi(argv[i])];
			}
		} else if(!strcasecmp(argv[i], "-char2")) {
			if(++i < argc) {
				g_main::poniko.fighter = &game::fighters[atoi(argv[i])];
			}
		} else if(!strcasecmp(argv[i], "-pal1")) {
			if(++i < argc) {
				g_main::madotsuki.palette = atoi(argv[i]);
			}
		} else if(!strcasecmp(argv[i], "-pal2")) {
			if(++i < argc) {
				g_main::poniko.palette = atoi(argv[i]);
			}
		} else if(!strcasecmp(argv[i], "-stage")) {
			if(++i < argc) {
				Stage::stage = atoi(argv[i]);
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

void init::optionsLoad() {
	File file;
	if(!file.open(File::FILE_READ_NORMAL, szConfigPath + "options.dat")) {
		return;
	}

    //If the options are newer, don't read them
    if((ubyte_t)file.readByte() > OPTIONS_VERSION) {
        return;
    }

	SceneOptions::optionDifficulty = file.readByte();
	SceneOptions::optionWins = file.readByte();
	SceneOptions::optionTime = file.readByte();
	SceneOptions::optionSfxVolume = file.readByte();
	SceneOptions::optionMusVolume = file.readByte();
	SceneOptions::optionVoiceVolume = file.readByte();
	SceneOptions::optionEpilepsy = file.readByte();
}

void init::optionsSave() {
	File file;
	if(!file.open(File::FILE_WRITE_NORMAL, szConfigPath + "options.dat")) {
		return;
	}

    file.writeByte(OPTIONS_VERSION);
	file.writeByte(SceneOptions::optionDifficulty);
	file.writeByte(SceneOptions::optionWins);
	file.writeByte(SceneOptions::optionTime);
	file.writeByte(SceneOptions::optionSfxVolume);
	file.writeByte(SceneOptions::optionMusVolume);
	file.writeByte(SceneOptions::optionVoiceVolume);
	file.writeByte(SceneOptions::optionEpilepsy);
}

void init::init() {
	atexit(deinit);

	srand(time(nullptr));

	//Create settings path first
	//Find correct directory
#if defined _WIN32
	constexpr auto CONFIG_PATH = "\\dreamvsdream\\";
	wchar_t* env16 = _wgetenv(L"AppData");
	if(!env16) {
		error::die("Cannot find settings directory.");
	}
	char* env = util::utf16to8(env16);
#elif defined __APPLE__
	constexpr auto CONFIG_PATH = "/Library/Application Support/dreamvsdream/";
	char* env = getenv("HOME");
	if(!env) {
		error::die("Cannot find settings directory.");
	}
#else
	constexpr auto CONFIG_PATH = "/dreamvsdream/";
	char* env = getenv("XDG_CONFIG_HOME");
	if(!env) {
		env = getenv("HOME");
		if(!env) {
			error::die("Cannot find settings directory.");
		}
	}
#endif

	//Allocate into new string
	szConfigPath = std::string(env) + CONFIG_PATH;

	//Create directory if it doesn't exist
#ifdef _WIN32
	free(env);
	wchar_t* szConfigPath16 = util::utf8to16(szConfigPath.c_str());
	CreateDirectoryW(szConfigPath16, nullptr);
	free(szConfigPath16);
#else
	mkdir(szConfigPath.c_str(), 0777);
#endif

	optionsLoad();

	os::init();
	graphics::init(disable_shaders, max_texture_size);
	Scene::imgLoading.createFromFile("scenes/loading.png");
	Scene::imgLoading.draw(0, 0);
	os::refresh();

	input::init();
	if(!disable_sound) {
		audio::init();
	}
	net::init(input_delay);
	game::init();
	Scene::ginit(); //TODO fix this
	Stage::ginit();
	effect::init();

	if(versus) {
		FIGHT->gametype = SceneFight::GAMETYPE_VERSUS;
	}
	//if(fullscreen)
	//OS::toggleFullscreen();

	if(Scene::scene == Scene::SCENE_FIGHT && Stage::stage == -1) {
		Stage::stage = 0;
	}
}

void init::deinit() {
    audio::deinit();

	effect::deinit();
	Stage::deinit();
	Scene::deinit();
	game::deinit();
	net::deinit();

	graphics::deinit();
	os::deinit();

	optionsSave();
}
