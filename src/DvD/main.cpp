// We have a main function
#define SDL_MAIN_HANDLED

#include "globals.h"
#include "player.h"
#include "fighter.h"
#include "graphics.h"
#include "error.h"
#include "scene.h"
#include "parser.h"
#include "stage.h"
#include "network.h"
#include "atlas.h"
#include "effect.h"
#include "animation.h"

game::Player madotsuki;
game::Player poniko;
util::Vector cameraPos(0,0);
util::Vector idealCameraPos(0,0);
util::Vector cameraShake(0,0);

extern game::Fighter fighters[];
extern Stage stages[];

int framePauseTimer = 0;
void pause(int frames) {
	framePauseTimer += frames;
}

int frameShakeTimer = 0;
void shake(int frames) {
	frameShakeTimer += frames;
}

extern void parseArgs(int, char**);

static inline void mainLoop() {
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

	parseArgs(argc, (char**)argv);

#ifdef _WIN32
	for(int i = 0; i < argc; i++) {
		free(argv[i]);
	}
	free(argv);
#endif
	extern void init();
	init();

	madotsuki.playerNum = 0;
	madotsuki.speaker.init();

	poniko.playerNum = 1;
	poniko.speaker.init();

	for(;;) {
		mainLoop();
	}

	return 0;
}
