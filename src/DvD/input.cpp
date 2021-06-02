#include "globals.h"
#include "player.h"
#include "input.h"
#include "scene/scene.h"
#include "network.h"

namespace g_main {
	extern game::Player madotsuki;
	extern game::Player poniko;
}

namespace input {
	enum {
		KEY_1_LEFT,
		KEY_1_RIGHT,
		KEY_1_UP,
		KEY_1_DOWN,
		KEY_1_A,
		KEY_1_B,
		KEY_1_C,

		KEY_2_LEFT,
		KEY_2_RIGHT,
		KEY_2_UP,
		KEY_2_DOWN,
		KEY_2_A,
		KEY_2_B,
		KEY_2_C,

		KEY_MAX
	};

	constexpr auto KEY_1_MAX = KEY_2_LEFT;
	constexpr auto KEY_2_MAX = KEY_MAX;

	int key_config[KEY_MAX] = {
		SDLK_LEFT,
		SDLK_RIGHT,
		SDLK_UP,
		SDLK_DOWN,
		SDLK_n,
		SDLK_m,
		SDLK_COMMA,

		SDLK_f,
		SDLK_h,
		SDLK_t,
		SDLK_g,
		SDLK_a,
		SDLK_s,
		SDLK_d,
	};

	constexpr auto KEY_CFG_FILE = "keys.cfg";

	void init() {
		//Read controls from file
		int nLines;
		char** szLines = util::getLinesFromFile(&nLines, init::szConfigPath + KEY_CFG_FILE);

		if(nLines == KEY_MAX) {
			for(int i = 0; i < KEY_MAX; i++) {
				if(strlen(szLines[i]) >= 2 && szLines[i][0] == '0' && szLines[i][1] == 'x') {
					key_config[i] = strtol((const char*)szLines[i], nullptr, 16);
				} else {
					key_config[i] = atoi(szLines[i]);
				}
			}
		}

		util::freeLines(szLines);
	}

	void refresh() {
		g_main::madotsuki.applyInput();
		g_main::poniko.applyInput();
		if(Scene::scene == 0) {
			g_main::madotsuki.handleInput();
			g_main::poniko.handleInput();
		}
	}

	void keyPress(int key, bool press) {
		if(press) {
			for(int i = 0; i < KEY_MAX; i++) {
				if(net::connected && i >= KEY_1_MAX) {
					break;
				}

				if(key_config[i] == key) {
					if(net::connected) {
						net::getMyPlayer()->frameInput |= (1<<i);
					} else {
						if(i < KEY_1_MAX) {
							g_main::madotsuki.frameInput |= (1<<i);
						} else {
							g_main::poniko.frameInput |= (1<<(i - KEY_1_MAX));
						}
					}
					return;
				}
			}

			switch(key) {
			case SDLK_F1:
				if(!net::connected) {
					Scene::setScene(Scene::SCENE_TITLE);
				}
				break;

			case SDLK_F2:
				if(!net::connected) {
					Scene::setScene(Scene::SCENE_CREDITS);
				}
				break;
			}
		} else {
			for(int i = 0; i < KEY_MAX; i++) {
				if(net::connected && i >= KEY_1_MAX) {
					break;
				}

				if(key_config[i] == key) {
					if(net::connected) {
						net::getMyPlayer()->frameInput |= (1<<i<<game::INPUT_RELSHIFT);
					} else {
						if(i < KEY_1_MAX) {
							g_main::madotsuki.frameInput |= (1<<i<< game::INPUT_RELSHIFT);
						} else {
							g_main::poniko.frameInput |= (1<<(i - KEY_1_MAX)<< game::INPUT_RELSHIFT);
						}
					}
					return;
				}
			}
		}
	}
}
