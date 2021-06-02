// We have a main function
#define SDL_MAIN_HANDLED

#include "globals.h"
#include "player.h"
#include "fighter.h"
#include "graphics.h"
#include "error.h"
#include "scene/scene.h"
#include "parser.h"
#include "stage.h"
#include "network.h"
#include "atlas.h"
#include "effect.h"
#include "animation.h"

namespace g_main {
	game::Player madotsuki;
	game::Player poniko;
	util::Vector cameraPos(0,0);
	util::Vector idealCameraPos(0,0);
	util::Vector cameraShake(0,0);

	int framePauseTimer = 0;
	int frameShakeTimer = 0;

	void pause(int frames);
	void shake(int frames);

	static inline void mainLoop();
}

void g_main::pause(int frames) {
	framePauseTimer += frames;
}

void g_main::shake(int frames) {
	frameShakeTimer += frames;
}

namespace init {
	extern void parseArgs(int, char**);
	extern void init();
}

static inline void g_main::mainLoop() {
	os::refresh();

	SCENE->think();
	SCENE->draw();
	Scene::drawFade();
}

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

	init::parseArgs(argc, (char**)argv);

#ifdef _WIN32
	for(int i = 0; i < argc; i++) {
		free(argv[i]);
	}
	free(argv);
#endif
	init::init();

	g_main::madotsuki.playerNum = 0;
	g_main::madotsuki.speaker.init();

	g_main::poniko.playerNum = 1;
	g_main::poniko.speaker.init();

	for(;;) {
		g_main::mainLoop();
	}

	return 0;
}
