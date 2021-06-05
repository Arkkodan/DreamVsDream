#include "app.h"

#include "error.h"
#include "file.h"
#include "scene/scene.h"
#include "scene/fight.h"
#include "scene/options.h"
#include "sys.h"
#include "graphics.h"
#include "input.h"
#include "sound.h"
#include "network.h"
#include "fighter.h"
#include "stage.h"
#include "effect.h"
#include "../util/fileIO.h"

#include <ctime>

#ifdef _WIN32
#ifndef WINVER
#define WINVER 0x0500
#endif
#include <windows.h>
#include <shlwapi.h>
#endif

namespace app {
	constexpr auto OPTIONS_VERSION = 0;

	std::string szConfigPath;

	bool disable_shaders = false;
	bool disable_sound = false;
	int max_texture_size = 0;
	bool versus = false;
	bool fullscreen = false;
	int input_delay = 0;

	static void optionsLoad();
	static void optionsSave();
	static void init();
	static void deinit();
}

void app::run() {
	app::init();

	scene::Fight::madotsuki.playerNum = 0;
	scene::Fight::madotsuki.speaker.init();

	scene::Fight::poniko.playerNum = 1;
	scene::Fight::poniko.speaker.init();

	for (;;) {
		sys::refresh();

		SCENE->think();
		SCENE->draw();
		scene::drawFade();
	}
}

static void app::optionsLoad() {
	File file;
	if (!file.open(File::FILE_READ_NORMAL, szConfigPath + "options.dat")) {
		return;
	}

	//If the options are newer, don't read them
	if ((uint8_t)file.readByte() > OPTIONS_VERSION) {
		return;
	}

	scene::Options::optionDifficulty = file.readByte();
	scene::Options::optionWins = file.readByte();
	scene::Options::optionTime = file.readByte();
	scene::Options::optionSfxVolume = file.readByte();
	scene::Options::optionMusVolume = file.readByte();
	scene::Options::optionVoiceVolume = file.readByte();
	scene::Options::optionEpilepsy = file.readByte();
}

static void app::optionsSave() {
	File file;
	if (!file.open(File::FILE_WRITE_NORMAL, szConfigPath + "options.dat")) {
		return;
	}

	file.writeByte(OPTIONS_VERSION);
	file.writeByte(scene::Options::optionDifficulty);
	file.writeByte(scene::Options::optionWins);
	file.writeByte(scene::Options::optionTime);
	file.writeByte(scene::Options::optionSfxVolume);
	file.writeByte(scene::Options::optionMusVolume);
	file.writeByte(scene::Options::optionVoiceVolume);
	file.writeByte(scene::Options::optionEpilepsy);
}

static void app::init() {
	atexit(deinit);

	srand(time(nullptr));

	//Create settings path first
	//Find correct directory
#if defined _WIN32
	constexpr auto CONFIG_PATH = "\\dreamvsdream\\";
	wchar_t* env16 = _wgetenv(L"AppData");
	if (!env16) {
		error::die("Cannot find settings directory.");
	}
	char* env = util::utf16to8(env16);
#elif defined __APPLE__
	constexpr auto CONFIG_PATH = "/Library/Application Support/dreamvsdream/";
	char* env = getenv("HOME");
	if (!env) {
		error::die("Cannot find settings directory.");
	}
#else
	constexpr auto CONFIG_PATH = "/dreamvsdream/";
	char* env = getenv("XDG_CONFIG_HOME");
	if (!env) {
		env = getenv("HOME");
		if (!env) {
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

	sys::init();
	graphics::init(disable_shaders, max_texture_size);
	scene::imgLoading.createFromFile("scenes/loading.png");
	scene::imgLoading.draw(0, 0);
	scene::scene = scene::SCENE_INTRO;
	sys::refresh();

	input::init();
	if (!disable_sound) {
		audio::init();
	}
	net::init(input_delay);
	game::init();
	scene::ginit(); //TODO fix this
	Stage::ginit();
	effect::init();

	if (versus) {
		FIGHT->gametype = scene::Fight::GAMETYPE_VERSUS;
	}
	//if(fullscreen)
	//OS::toggleFullscreen();

	if (scene::scene == scene::SCENE_FIGHT && Stage::stage == -1) {
		Stage::stage = 0;
	}
}

static void app::deinit() {
	audio::deinit();

	effect::deinit();
	Stage::deinit();
	scene::deinit();
	game::deinit();
	net::deinit();

	graphics::deinit();
	sys::deinit();

	optionsSave();
}
