#include "options.h"

#include "scene.h"

#include "../graphics.h"
#include "../../util/rng.h"

#include <cstring>

int SceneOptions::optionDifficulty = 3;
int SceneOptions::optionWins = 2;
int SceneOptions::optionTime = 99;
int SceneOptions::optionSfxVolume = 100;
int SceneOptions::optionMusVolume = 100;
int SceneOptions::optionVoiceVolume = 100;
bool SceneOptions::optionEpilepsy = false;

SceneOptions::SceneOptions() : Scene("options") {
	cursor = cursorLast = cursorTimer = 0;
	madoPos = 0;
	madoFrame = 1;
	madoDir = 2;
	madoWakeTimer = 0;
	themes.clear();
	nThemes = 0;
	iR = iG = iB = aR = aG = aB = 0;
	aXOffset = 0;
}

SceneOptions::~SceneOptions() {}

void SceneOptions::think() {
	Scene::think();

	//Cursor stuff
	if (cursorTimer) {
		if (cursorTimer == 1 || cursorTimer == -1) {
			cursorTimer = 0;
		}
		else {
			cursorTimer /= 2;
		}
	}

	//Move/animate Madotsuki
	if (madoWakeTimer) {
		if (madoWakeTimer == 1) {
			madoWakeTimer = -1;
			setScene(SCENE_TITLE);
		}
		else {
			madoWakeTimer--;
			if (madoWakeTimer == 36) {
				madoFrame++;
			}
			else if (madoWakeTimer == 32) {
				madoSfxPinch.play();
				madoFrame++;
			}
		}
	}
	else {
		bool stopped = false;
		if (madoPos < cursor * 32) {
			//Move down
			madoDir = 2;
			madoPos++;
			if (madoPos % 8 == 0) {
				madoFrame++;
				if (madoFrame >= 4) {
					madoFrame = 0;
				}
				if (madoFrame % 2 == 1) {
					madoSfxStep.play();
				}
			}
		}
		else if (madoPos > cursor * 32) {
			//Move up
			madoDir = 0;
			madoPos--;
			if (madoPos % 8 == 0) {
				madoFrame++;
				if (madoFrame >= 4) {
					madoFrame = 0;
				}
				if (madoFrame % 2 == 1) {
					madoSfxStep.play();
				}
			}
		}
		else {
			madoFrame = 1;
			stopped = true;
		}

		if (input(game::INPUT_UP | game::INPUT_DOWN)) {
			cursorTimer = aXOffset;
			cursorLast = cursor;
		}

		if (input(game::INPUT_UP)) {
			sndMenu.play();
			if (cursor) {
				cursor--;
			}
			else {
				cursor = OPTION_MAX - 1;
			}
		}
		else if (input(game::INPUT_DOWN)) {
			sndMenu.play();
			if (cursor < OPTION_MAX - 1) {
				cursor++;
			}
			else {
				cursor = 0;
			}
		}
		if (stopped) {
			if (input(game::INPUT_LEFT)) {
				madoDir = 3;
			}
			else if (input(game::INPUT_RIGHT)) {
				madoDir = 1;
			}
		}

		//Change option
		if (input(game::INPUT_LEFT) || input(game::INPUT_RIGHT)) {
			if (cursor != OPTION_VOICE_VOLUME) {
				sndMenu.play();
			}

			switch (cursor) {
			case OPTION_DIFFICULTY:
				if (input(game::INPUT_LEFT)) {
					if (optionDifficulty > 1) {
						optionDifficulty--;
					}
				}
				else {
					if (optionDifficulty < 5) {
						optionDifficulty++;
					}
				}
				break;

			case OPTION_WINS:
				if (input(game::INPUT_LEFT)) {
					if (optionWins > 1) {
						optionWins--;
					}
				}
				else {
					if (optionWins < 3) {
						optionWins++;
					}
				}
				break;

			case OPTION_TIME:
				if (input(game::INPUT_LEFT)) {
					if (optionTime == 0) {
						optionTime = 99;
					}
					else if (optionTime == 99) {
						optionTime = 60;
					}
				}
				else {
					if (optionTime == 60) {
						optionTime = 99;
					}
					else if (optionTime == 99) {
						optionTime = 0;
					}
				}
				break;

			case OPTION_SFX_VOLUME:
				if (input(game::INPUT_LEFT)) {
					if (optionSfxVolume > 0) {
						optionSfxVolume -= 10;
					}
				}
				else {
					if (optionSfxVolume < 100) {
						optionSfxVolume += 10;
					}
				}
				break;

			case OPTION_MUS_VOLUME:
				if (input(game::INPUT_LEFT)) {
					if (optionMusVolume > 0) {
						optionMusVolume -= 10;
					}
				}
				else {
					if (optionMusVolume < 100) {
						optionMusVolume += 10;
					}
				}
				break;

			case OPTION_VOICE_VOLUME:
				if (input(game::INPUT_LEFT)) {
					if (optionVoiceVolume > 0) {
						optionVoiceVolume -= 10;
					}
					SceneFight::madotsuki.speaker.play(&dame);
				}
				else {
					if (optionVoiceVolume < 100) {
						optionVoiceVolume += 10;
					}
					SceneFight::madotsuki.speaker.play(&muri);
				}
				break;

			case OPTION_EPILEPSY:
				if (input(game::INPUT_LEFT | game::INPUT_RIGHT)) {
					optionEpilepsy = !optionEpilepsy;
				}
				break;
			}
		}

		if (input(game::INPUT_A)) {
			if (cursor == OPTION_CREDITS) {
				sndSelect.play();
				setScene(SCENE_CREDITS);
			}
		}

		//Pinch thyself awake, Madotsuki
		if (input(game::INPUT_B)) {
			madoDir = 4;
			madoFrame = 0;
			madoWakeTimer = 40;
		}
	}
}

void SceneOptions::reset() {
	Scene::reset();

	cursor = cursorLast = cursorTimer = 0;
	madoPos = 0;
	madoFrame = 1;
	madoDir = 2;
	madoWakeTimer = 0;
}

void SceneOptions::draw() const {
	Scene::draw();

	//Draw the menu options
	if (menuFont.exists()) {
		for (int i = 0; i < OPTION_MAX; i++) {
			char buff[80];
			switch (i) {
			case OPTION_DIFFICULTY:
				sprintf(buff, "Difficulty:\t%d", optionDifficulty);
				break;

			case OPTION_WINS:
				sprintf(buff, "Wins:\t%d", optionWins);
				break;

			case OPTION_TIME:
				if (optionTime) {
					sprintf(buff, "Time:\t%d", optionTime);
				}
				else {
					sprintf(buff, "Time:\tUnlimited");
				}
				break;

			case OPTION_SFX_VOLUME:
				sprintf(buff, "Sound Volume:\t%d%%", optionSfxVolume);
				break;

			case OPTION_MUS_VOLUME:
				sprintf(buff, "Music Volume:\t%d%%", optionMusVolume);
				break;

			case OPTION_VOICE_VOLUME:
				sprintf(buff, "Voice Volume:\t%d%%", optionVoiceVolume);
				break;

			case OPTION_EPILEPSY:
				if (optionEpilepsy) {
					strcpy(buff, "Photosensitivity Mode:\tOn");
				}
				else {
					strcpy(buff, "Photosensitivity Mode:\tOff");
				}
				break;

			case OPTION_CREDITS:
				strcpy(buff, "Credits");
				break;
			}

			if (i == cursor) {
				menuFont.drawText(64 + (aXOffset - cursorTimer), 64 + i * 32, buff, aR, aG, aB);
			}
			else if (i == cursorLast) {
				menuFont.drawText(64 + cursorTimer, 64 + i * 32, buff, iR, iG, iB);
			}
			else {
				menuFont.drawText(64, 64 + i * 32, buff, iR, iG, iB);
			}
		}
	}

	int xoff = madoFrame * 16;
	if (madoFrame == 3) {
		xoff = 16;
	}
	graphics::setRect(xoff, madoDir * 32, 16, 32);
	madoImg.draw(32, 64 + madoPos);
}

void SceneOptions::init() {
	Scene::init();

	//Parse a random theme
	if (nThemes) {
		parseFile(getResource(themes[util::roll(nThemes)], Parser::EXT_SCRIPT));
	}
}

void SceneOptions::parseLine(Parser& parser) {
	int argc = parser.getArgC();
	if (parser.is("FONT", 1)) {
		//The font
		menuFont.createFromFile(getResource(parser.getArg(1), Parser::EXT_FONT));
	}
	else if (parser.is("MADOTSUKI", 3)) {
		//Madotsuki sprites/sounds
		madoImg.createFromFile(getResource(parser.getArg(1), Parser::EXT_IMAGE));
		madoSfxStep.createFromFile(getResource(parser.getArg(2), Parser::EXT_SOUND));
		madoSfxPinch.createFromFile(getResource(parser.getArg(3), Parser::EXT_SOUND));
	}
	else if (parser.is("INACTIVE", 3)) {
		iR = parser.getArgInt(1);
		iG = parser.getArgInt(2);
		iB = parser.getArgInt(3);
	}
	else if (parser.is("ACTIVE", 3)) {
		aR = parser.getArgInt(1);
		aG = parser.getArgInt(2);
		aB = parser.getArgInt(3);

		if (argc > 4) {
			aXOffset = parser.getArgInt(4);
		}
	}
	else if (parser.is("THEMES", 1)) {
		nThemes = 0;
		themes.resize(parser.getArgInt(1));
	}
	else if (parser.is("THEME", 1)) {
		themes[nThemes++] = parser.getArg(1);
	}
	else if (parser.is("VOICES", 2)) {
		dame.createFromFile(getResource(parser.getArg(1), Parser::EXT_SOUND));
		muri.createFromFile(getResource(parser.getArg(2), Parser::EXT_SOUND));
	}
	else {
		Scene::parseLine(parser);
	}
}
