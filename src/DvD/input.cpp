#include "input.h"

#include "player.h"
#include "scene/scene.h"
#include "scene/fight.h"
#include "network.h"
#include "app.h"
#include "../fileIO/text.h"

#include <array>
#include <cstring>

#include <SDL_keycode.h>

namespace input {
	enum class Key {
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

	constexpr auto KEY_1_MAX = Key::KEY_2_LEFT;
	constexpr auto KEY_2_MAX = Key::KEY_MAX;

	std::array<int, static_cast<size_t>(Key::KEY_MAX)> key_config = {
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
		auto lines = fileIO::readTextAsLines(app::szConfigPath + KEY_CFG_FILE);
		if (lines.size() == static_cast<int>(Key::KEY_MAX)) {
			for (int i = 0; i < static_cast<int>(Key::KEY_MAX); i++) {
				const auto& line = lines[i];
				if (line.size() >= 2 && line.substr(0, 2) == "0x") {
					key_config[i] = std::strtol(line.c_str(), nullptr, 16);
				}
				else {
					key_config[i] = std::strtol(line.c_str(), nullptr, 10);
				}
			}
		}
	}

	void refresh() {
		scene::Fight::madotsuki.applyInput();
		scene::Fight::poniko.applyInput();
		if (scene::scene == scene::SCENE_FIGHT) {
			scene::Fight::madotsuki.handleInput();
			scene::Fight::poniko.handleInput();
		}
	}

	void keyPress(int key, bool press) {
		if(press) {
			for(int i = 0; i < static_cast<int>(Key::KEY_MAX); i++) {
				if(net::connected && i >= static_cast<int>(KEY_1_MAX)) {
					break;
				}

				if(key_config[i] == key) {
					if(net::connected) {
						net::getMyPlayer()->frameInput |= (1<<i);
					} else {
						if(i < static_cast<int>(KEY_1_MAX)) {
							scene::Fight::madotsuki.frameInput |= (1<<i);
						} else {
							scene::Fight::poniko.frameInput |= (1<<(i - static_cast<int>(KEY_1_MAX)));
						}
					}
					return;
				}
			}

			switch(key) {
			case SDLK_F1:
				if(!net::connected) {
					scene::setScene(scene::SCENE_TITLE);
				}
				break;

			case SDLK_F2:
				if(!net::connected) {
					scene::setScene(scene::SCENE_CREDITS);
				}
				break;
			}
		} else {
			for(int i = 0; i < static_cast<int>(Key::KEY_MAX); i++) {
				if(net::connected && i >= static_cast<int>(KEY_1_MAX)) {
					break;
				}

				if(key_config[i] == key) {
					if(net::connected) {
						net::getMyPlayer()->frameInput |= (1<<i<<game::INPUT_RELSHIFT);
					} else {
						if(i < static_cast<int>(KEY_1_MAX)) {
							scene::Fight::madotsuki.frameInput |= (1<<i<< game::INPUT_RELSHIFT);
						} else {
							scene::Fight::poniko.frameInput |= (1<<(i - static_cast<int>(KEY_1_MAX))<< game::INPUT_RELSHIFT);
						}
					}
					return;
				}
			}
		}
	}
}
