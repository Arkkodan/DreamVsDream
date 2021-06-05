#include "app.h"

// parseArgs not useful for now
#undef DVD_DISABLE_PARSEARGS

#ifndef DVD_DISABLE_PARSEARGS
#include "util.h"
#include "scene/fight.h"
#include "player.h"
#include "fighter.h"
#include "stage.h"

#ifdef _WIN32
#ifndef WINVER
#define WINVER 0x0500
#endif
#include <windows.h>
#include <shlwapi.h>
#endif

#if(_MSC_VER) // MSVC uses _stricmp instead of strcasecmp
#define strcasecmp(str1, str2) _stricmp(str1, str2)
#endif

static void parseArgs(int argc, char** argv);

#ifdef _WIN32
int main(int foo, char** bar)
#else
int main(int argc, char** argv)
#endif
{
#ifdef _WIN32
	int argc;
	wchar_t** argv16 = CommandLineToArgvW(GetCommandLineW(), &argc);
	char** argv = (char**)malloc(argc * sizeof(char*));
	for(int i = 0; i < argc; i++) {
		argv[i] = util::utf16to8(argv16[i]);
	}
	LocalFree(argv16);
#endif

	parseArgs(argc, (char**)argv);

#ifdef _WIN32
	for(int i = 0; i < argc; i++) {
		free(argv[i]);
	}
	free(argv);
#endif
	
	app::run();

	return 0;
}

static void parseArgs(int argc, char** argv) {
	SceneFight::madotsuki.fighter = &game::fighters[0];
	SceneFight::poniko.fighter = &game::fighters[0];

	for (int i = 1; i < argc; i++) {
		if (!strcasecmp(argv[i], "--disable-shaders")) {
			app::disable_shaders = true;
		}
		else if (!strcasecmp(argv[i], "--disable-sound")) {
			app::disable_sound = true;
		}
		else if (!strcasecmp(argv[i], "--training")) {
			Scene::scene = Scene::SCENE_FIGHT;
		}
		else if (!strcasecmp(argv[i], "--versus")) {
			Scene::scene = Scene::SCENE_FIGHT;
			app::versus = true;
		}
		else if (!strcasecmp(argv[i], "--fullscreen")) {
			app::fullscreen = true;
		}
		else if (!strcasecmp(argv[i], "-char1")) {
			if (++i < argc) {
				SceneFight::madotsuki.fighter = &game::fighters[atoi(argv[i])];
			}
		}
		else if (!strcasecmp(argv[i], "-char2")) {
			if (++i < argc) {
				SceneFight::poniko.fighter = &game::fighters[atoi(argv[i])];
			}
		}
		else if (!strcasecmp(argv[i], "-pal1")) {
			if (++i < argc) {
				SceneFight::madotsuki.palette = atoi(argv[i]);
			}
		}
		else if (!strcasecmp(argv[i], "-pal2")) {
			if (++i < argc) {
				SceneFight::poniko.palette = atoi(argv[i]);
			}
		}
		else if (!strcasecmp(argv[i], "-stage")) {
			if (++i < argc) {
				Stage::stage = atoi(argv[i]);
			}
		}
		else if (!strcasecmp(argv[i], "-input-delay")) {
			if (++i < argc) {
				app::input_delay = atoi(argv[i]);
			}
		}
		else if (!strcasecmp(argv[i], "-max-texture-size")) {
			if (++i < argc) {
				app::max_texture_size = atoi(argv[i]);
			}
		}
	}
}
#else // DVD_DISABLE_PARSEARGS
int main(int argc, char* argv[]) {
	app::run();
	return 0;
}

#endif // DVD_DISABLE_PARSEARGS
