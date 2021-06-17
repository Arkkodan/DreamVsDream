#include "app.h"

#include "../util/fileIO.h"
#include "effect.h"
#include "error.h"
#include "fighter.h"
#include "file.h"
#include "graphics.h"
#include "input.h"
#include "network.h"
#include "scene/fight.h"
#include "scene/options.h"
#include "scene/scene.h"
#include "shader_renderer/texture2D_renderer.h"
#include "sound.h"
#include "stage.h"
#include "sys.h"

#include <ctime>

#ifdef _WIN32
#ifndef WINVER
#define WINVER 0x0500
#endif
#include <shlwapi.h>
#include <windows.h>
#endif

namespace app {
  static constexpr auto OPTIONS_VERSION = 0;

  static std::string szConfigPath;

  static bool disable_shaders = false;
  static bool disable_sound = false;
  static int max_texture_size = 0;
  static bool versus = false;
  static bool fullscreen = false;
  static int input_delay = 0;

  static void optionsLoad();
  static void optionsSave();
  static void init();
  static void deinit();
} // namespace app

void app::run() {
  app::init();

  scene::Fight::madotsuki.setPlayerNumber(0);
  scene::Fight::madotsuki.getrSpeaker().init();

  scene::Fight::poniko.setPlayerNumber(1);
  scene::Fight::poniko.getrSpeaker().init();

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

  // If the options are newer, don't read them
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

  // Create settings path first
  // Find correct directory
#if defined _WIN32
  constexpr auto CONFIG_PATH = "\\dreamvsdream\\";
  wchar_t *env16 = _wgetenv(L"AppData");
  if (!env16) {
    error::die("Cannot find settings directory.");
  }
  char *env = util::utf16to8(env16);
#elif defined __APPLE__
  constexpr auto CONFIG_PATH = "/Library/Application Support/dreamvsdream/";
  char *env = getenv("HOME");
  if (!env) {
    error::die("Cannot find settings directory.");
  }
#else
  constexpr auto CONFIG_PATH = "/dreamvsdream/";
  char *env = getenv("XDG_CONFIG_HOME");
  if (!env) {
    env = getenv("HOME");
    if (!env) {
      error::die("Cannot find settings directory.");
    }
  }
#endif

  // Allocate into new string
  szConfigPath = std::string(env) + CONFIG_PATH;

  // Create directory if it doesn't exist
#ifdef _WIN32
  free(env);
  wchar_t *szConfigPath16 = util::utf8to16(szConfigPath.c_str());
  CreateDirectoryW(szConfigPath16, nullptr);
  free(szConfigPath16);
#else
  mkdir(szConfigPath.c_str(), 0777);
#endif

  optionsLoad();

  sys::init();
  graphics::init(disable_shaders, max_texture_size);
  Image *imgLoading = scene::getLoadingImage();
  imgLoading->createFromFile("scenes/loading.png");
  imgLoading->draw<renderer::Texture2DRenderer>(0, 0);
  scene::setIMSceneIndex(scene::SCENE_INTRO);
  sys::refresh();

  input::init();
  if (!disable_sound) {
    audio::init();
  }
  net::init(input_delay);
  game::init();
  scene::ginit(); // TODO fix this
  Stage::ginit();
  effect::init();

  if (versus) {
    FIGHT->gametype = scene::Fight::GAMETYPE_VERSUS;
  }
  // if(fullscreen)
  // OS::toggleFullscreen();

  if (scene::getSceneIndex() == scene::SCENE_FIGHT &&
      Stage::getStageIndex() == -1) {
    Stage::setStageIndex(0);
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

std::string app::getConfigPath() { return szConfigPath; }
