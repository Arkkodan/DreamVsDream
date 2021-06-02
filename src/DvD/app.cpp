#include "app.h"

#include "file.h"
#include "scene/scene.h"
#include "error.h"
#include "input.h"
#include "network.h"
#include "stage.h"
#include "effect.h"

#include "sys.h"

#include <ctime>

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

	SceneFight::madotsuki.playerNum = 0;
	SceneFight::madotsuki.speaker.init();

	SceneFight::poniko.playerNum = 1;
	SceneFight::poniko.speaker.init();

	for (;;) {
		sys::refresh();

		SCENE->think();
		SCENE->draw();
		Scene::drawFade();
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

	SceneOptions::optionDifficulty = file.readByte();
	SceneOptions::optionWins = file.readByte();
	SceneOptions::optionTime = file.readByte();
	SceneOptions::optionSfxVolume = file.readByte();
	SceneOptions::optionMusVolume = file.readByte();
	SceneOptions::optionVoiceVolume = file.readByte();
	SceneOptions::optionEpilepsy = file.readByte();
}

static void app::optionsSave() {
	File file;
	if (!file.open(File::FILE_WRITE_NORMAL, szConfigPath + "options.dat")) {
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
	Scene::imgLoading.createFromFile("scenes/loading.png");
	Scene::imgLoading.draw(0, 0);
	sys::refresh();

	input::init();
	if (!disable_sound) {
		audio::init();
	}
	net::init(input_delay);
	game::init();
	Scene::ginit(); //TODO fix this
	Stage::ginit();
	effect::init();

	if (versus) {
		FIGHT->gametype = SceneFight::GAMETYPE_VERSUS;
	}
	//if(fullscreen)
	//OS::toggleFullscreen();

	if (Scene::scene == Scene::SCENE_FIGHT && Stage::stage == -1) {
		Stage::stage = 0;
	}
}

static void app::deinit() {
	audio::deinit();

	effect::deinit();
	Stage::deinit();
	Scene::deinit();
	game::deinit();
	net::deinit();

	graphics::deinit();
	sys::deinit();

	optionsSave();
}
