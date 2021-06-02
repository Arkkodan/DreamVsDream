#include "title.h"

#include "scene.h"

//TITLE

enum {
	TM_MAIN,
	TM_VERSUS,

	TM_MAX
};

enum {
	CHOICE_ARCADE,
	CHOICE_STORY,
	CHOICE_VERSUS,
	CHOICE_SURVIVAL,
	CHOICE_TRAINING,
#ifndef NO_NETWORK
	CHOICE_NETPLAY,
#endif
	CHOICE_OPTIONS,
	CHOICE_QUIT,

	CHOICE_MAX
};

enum {
	CHOICE_VS_PLR,
	CHOICE_VS_CPU,
	CHOICE_VS_TAG,
	CHOICE_VS_TEAM,

	CHOICE_VS_RETURN,

	CHOICE_VS_MAX
};

const char* menuChoicesMain[CHOICE_MAX] = {
	"Arcade",
	"Story",
	"Versus",
	"Survival",
	"Training",
#ifndef NO_NETWORK
	"Netplay",
#endif
	"Options",
	"Quit",
};

const char* menuChoicesVersus[CHOICE_VS_MAX] = {
	"Versus Player",
	"Versus CPU",
	"Tag Team",
	"2v2 Team",
	"Return",
};

const char** menuChoices[TM_MAX] = {
	menuChoicesMain,
	menuChoicesVersus,
};

const int menuChoicesMax[TM_MAX] = {
	CHOICE_MAX,
	CHOICE_VS_MAX,
};

SceneTitle::SceneTitle() : Scene("title") {
	menuX = menuY = 0;
	menuXOffset = 0;
	iR = iG = iB = aR = aG = aB = 255;
	aXOffset = 0;

	choice = choiceLast = choiceTimer = 0;
	submenu = 0;

	nThemes = 0;
	themes = nullptr;
}

SceneTitle::~SceneTitle() {
	delete[] themes;
}

void SceneTitle::init() {
	Scene::init();

	//Parse a random theme
	if (nThemes) {
		//util::roll a weighted random number, favoring the "normal" title
		int i = util::roll(nThemes + 5);
		i -= 5;
		if (i < 0) {
			i = 0;
		}

		parseFile(getResource(themes[i], EXT_SCRIPT));
	}
}

void SceneTitle::think() {
	Scene::think();

	if (choiceTimer) {
		if (choiceTimer == 1 || choiceTimer == -1) {
			choiceTimer = 0;
		}
		else {
			choiceTimer /= 2;
		}
	}

	uint16_t up = INPUT_UP;
	uint16_t down = INPUT_DOWN;

	if (menuXOffset < 0) {
		up |= INPUT_RIGHT;
		down |= INPUT_LEFT;
	}
	else {
		up |= INPUT_LEFT;
		down |= INPUT_RIGHT;
	}

	if (input(up)) {
		sndMenu.play();

		choiceTimer = aXOffset;
		choiceLast = choice;
		if (choice == 0) {
			choice = menuChoicesMax[submenu] - 1;
		}
		else {
			choice--;
		}
	}
	else if (input(down)) {
		sndMenu.play();

		choiceTimer = aXOffset;
		choiceLast = choice;
		if (choice >= menuChoicesMax[submenu] - 1) {
			choice = 0;
		}
		else {
			choice++;
		}
	}
	else if (input(INPUT_A)) {
		//Enter the new menu
		switch (submenu) {
		case TM_MAIN:
			switch (choice) {
			case CHOICE_VERSUS:
				sndSelect.play();
				choice = 0;
				choiceLast = 0;
				choiceTimer = 0;
				submenu = TM_VERSUS;
				break;
			case CHOICE_TRAINING:
				sndSelect.play();
				FIGHT->gametype = GAMETYPE_TRAINING;
				setScene(SCENE_SELECT);
				break;
			default:
				//if(sndSelect) sndSelect->play();
				//setScene(SCENE_SELECT);
				sndInvalid.play();
				break;
#ifndef NO_NETWORK
			case CHOICE_NETPLAY:
				sndSelect.play();
				setScene(SCENE_NETPLAY);
				break;
#endif
			case CHOICE_OPTIONS:
				sndSelect.play();
				setScene(SCENE_OPTIONS);
				break;
			case CHOICE_QUIT:
				setScene(SCENE_QUIT);
				break;
			}
			break;

		case TM_VERSUS:
			switch (choice) {
			case CHOICE_VS_PLR:
				sndSelect.play();
				FIGHT->gametype = GAMETYPE_VERSUS;
				setScene(SCENE_SELECT);
				break;
			default:
				//if(sndSelect) sndSelect->play();
				//setScene(SCENE_SELECT);
				sndInvalid.play();
				break;
			case CHOICE_VS_RETURN:
				sndBack.play();
				choice = CHOICE_VERSUS;
				choiceLast = choice;
				choiceTimer = 0;
				submenu = TM_MAIN;
				break;
			}
			break;
		}
	}
	else if (input(INPUT_B)) {
		if (submenu == TM_MAIN) {
			//Quit
			setScene(SCENE_QUIT);
		}
		else if (submenu == TM_VERSUS) {
			//Return
			sndBack.play();
			choice = CHOICE_VERSUS;
			choiceLast = choice;
			choiceTimer = 0;
			submenu = TM_MAIN;
		}
	}
}

void SceneTitle::reset() {
	Scene::reset();
}

void SceneTitle::draw() {
	Scene::draw();

	if (menuFont.exists()) {
		for (int i = 0; i < menuChoicesMax[submenu]; i++) {
			int gray = 2;
			if (submenu == TM_MAIN) {
				switch (i) {
				case CHOICE_VERSUS:
				case CHOICE_TRAINING:
				case CHOICE_OPTIONS:
#ifndef NO_NETWORK
				case CHOICE_NETPLAY:
#endif
				case CHOICE_QUIT:
					gray = 1;
					break;
				}
			}
			else if (submenu == TM_VERSUS) {
				switch (i) {
				case CHOICE_VS_PLR:
				case CHOICE_VS_RETURN:
					gray = 1;
					break;
				}
			}

			if (i == choice) {
				menuFont.drawText(menuX + i * menuXOffset + (aXOffset - choiceTimer), menuY + menuFont.img.h * i, menuChoices[submenu][i], aR / gray, aG / gray, aB / gray, 255);
			}
			else if (i == choiceLast) {
				menuFont.drawText(menuX + i * menuXOffset + choiceTimer, menuY + menuFont.img.h * i, menuChoices[submenu][i], iR / gray, iG / gray, iB / gray, 255);
			}
			else {
				menuFont.drawText(menuX + i * menuXOffset, menuY + menuFont.img.h * i, menuChoices[submenu][i], iR / gray, iG / gray, iB / gray, 255);
			}
		}
	}
}

void SceneTitle::parseLine(Parser& parser) {
	int argc = parser.getArgC();
	if (parser.is("MENU", 3)) {
		//Font
		menuFont.createFromFile(getResource(parser.getArg(1), EXT_FONT));

		menuX = parser.getArgInt(2);
		menuY = parser.getArgInt(3);

		if (argc > 4) {
			menuXOffset = parser.getArgInt(4);
		}
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

		if (argc >= 4) {
			aXOffset = parser.getArgInt(4);
		}
	}
	else if (parser.is("THEMES", 1)) {
		nThemes = 0;
		themes = new std::string[parser.getArgInt(1)];
	}
	else if (parser.is("THEME", 1)) {
		themes[nThemes++] = parser.getArg(1);
	}
	else {
		Scene::parseLine(parser);
	}
}
