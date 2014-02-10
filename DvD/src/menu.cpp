#include "globals.h"
#include "util.h"
#include "menu.h"
#include "player.h"
#include "stage.h"
#include "fighter.h"
#include "graphics.h"
#include "network.h"

#include "parser.h"
#include "util.h"

#ifdef EMSCRIPTEN
#include <SDL/SDL_mixer.h>
#endif

#define PORTRAIT_FADE 50

extern game::Player madotsuki;
extern game::Player poniko;

Menu* menus[MENU_MAX] = {nullptr};
int menu = MENU_INTRO;
int menuNew = 0;

Image imgLoading;

void Menu::ginit() {
	menus[MENU_INTRO] = new MenuIntro();
	menus[MENU_TITLE] = new MenuTitle();
	menus[MENU_SELECT] = new MenuSelect();
	menus[MENU_VERSUS] = new MenuVersus();
	menus[MENU_FIGHT] = new MenuFight();
	menus[MENU_OPTIONS] = new MenuOptions();
#ifndef NO_NETWORK
	menus[MENU_NETPLAY] = new MenuNetplay();
#endif
	menus[MENU_CREDITS] = new MenuCredits();

	menus[MENU_FIGHT]->init();
	menus[MENU_VERSUS]->init();
	menus[MENU_INTRO]->init();
}

void Menu::deinit() {
	for(int i = 0; i < MENU_MAX; i++) {
		delete menus[i];
	}
}

void Menu::setMenu(int _menu) {
	if(_menu == menu) {
		return;
	}
	menuNew = _menu;
	fade = 1.0f;
	fadeIn = false;
}

bool Menu::input(uint16_t in) {
	return (madotsuki.frameInput & in) || (poniko.frameInput & in);
}

float Menu::fade = 1.0f;
bool Menu::fadeIn = true;

void Menu::drawFade() {
	//Draw fade!
	glBindTexture(GL_TEXTURE_2D, 0);
	if(fadeIn) {
		glColor4f(0.0f, 0.0f, 0.0f, fade);
	} else {
		glColor4f(0.0f, 0.0f, 0.0f, 1.0f - fade);
	}
	glBegin(GL_QUADS);
	glVertex3f(0.0f, 0.0f, 0.0f);
	glVertex3f(0.0f, WINDOW_HEIGHT, 0.0f);
	glVertex3f(WINDOW_WIDTH, WINDOW_HEIGHT, 0.0f);
	glVertex3f(WINDOW_WIDTH, 0.0f, 0.0f);
	glEnd();
	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
}

//MENUS
Menu::Menu(std::string name_) {
	//Copy the name.
	name = name_;

	initialized = false;
	images = nullptr;
	bgmPlaying = false;
	//video = nullptr;
}

Menu::~Menu() {
	delete images;
	//delete video;
}

void Menu::init() {
	initialized = true;
	//Load the menu data from the file
	parseFile("menus/" + name + ".ubu");
}

void Menu::think() {
	if(!initialized) {
		init();
	}

	if(!bgmPlaying &&
#ifndef NO_NETWORK
	        menu != MENU_NETPLAY &&
#endif
	        menu != MENU_FIGHT) {
		if(bgm.exists()) {
			bgm.play();
		}
		bgmPlaying = true;
	}
	if(images) {
		images->think();
	}
	//if(video) video->think();

	//Fade timer
	if(fade) {
		if(fadeIn && menu == MENU_VERSUS) {
			fade = 0.0f;
		} else {
			if(menu == MENU_FIGHT) {
				fade -= 0.02;
			} else {
				fade -= 0.1f;
			}
			if(fade <= 0.0f) {
				if(fadeIn) {
					fade = 0.0f;
				} else {
					fade = 1.0f;
					fadeIn = true;

					//Are we quitting?
#ifndef EMSCRIPTEN
					if(menuNew == MENU_QUIT) {
						exit(0);
					}
#endif
					menu = menuNew;

					audio::Music::stop();

					if(!MENU->initialized) {
						//Loading graphic
						imgLoading.draw(0, 0);
						os::refresh();
						MENU->init();
					}
					if(menu == MENU_FIGHT && !STAGE.initialized) {
						//Loading graphic
						imgLoading.draw(0, 0);
						os::refresh();
						STAGE.init();
					}
					MENU->reset();
				}
			}

			//Always disable controls during fades
			madotsuki.frameInput = 0;
		}
	}
}

void Menu::reset() {
	if(images) {
		images->reset();
	}
	//if(video) video->reset();
	bgmPlaying = false;
}

void Menu::draw() {
	//if(video) video->draw(0, 0);
	if(images) {
		images->draw(false);
	}


}

void Menu::parseLine(Parser& parser) {
	int argc = parser.getArgC();
	if(parser.is("IMAGE", 3)) {
		float x = parser.getArgFloat(2);
		float y = parser.getArgFloat(3);
		char render = RENDER_NORMAL;
		float xvel = 0.0f;
		float yvel = 0.0f;
		bool wrap = false;
		if(argc > 4) {
			const char* szRender = parser.getArg(4);
			if(!strcmp(szRender, "additive")) {
				render = RENDER_ADDITIVE;
			} else if(!strcmp(szRender, "subtractive")) {
				render = RENDER_SUBTRACTIVE;
			} else if(!strcmp(szRender, "multiply")) {
				render = RENDER_MULTIPLY;
			}
			if(argc > 5) {
				xvel = parser.getArgFloat(5);
				if(argc > 6) {
					yvel = parser.getArgFloat(6);
					if(argc > 7) {
						wrap = parser.getArgBool(7, false);
					}
				}
			}
		}

		//Add a new image
		Image imgData;
		imgData.createFromFile(getResource(parser.getArg(1), EXT_IMAGE));
		if(!imgData.exists()) {
			return;
		}
		MenuImage* newImg = new MenuImage(imgData, x, y, 1.0f, render, xvel, yvel, wrap, 0);

		if(images) {
			MenuImage* img = images;
			for(; img->next; img = img->next);
			img->next = newImg;
		} else {
			images = newImg;
		}
	} else if(parser.is("BGM", 1)) {
		if(argc > 2) {
			std::string intro = getResource(parser.getArg(1), EXT_MUSIC);
			std::string loop = getResource(parser.getArg(2), EXT_MUSIC);
			bgm.createFromFile(intro, loop);
		} else {
			bgm.createFromFile("", getResource(parser.getArg(1), EXT_MUSIC));
		}
	} else if(parser.is("SOUND", 4)) {
		sndMenu.createFromFile(getResource(parser.getArg(1), EXT_SOUND));
		sndSelect.createFromFile(getResource(parser.getArg(2), EXT_SOUND));
		sndBack.createFromFile(getResource(parser.getArg(3), EXT_SOUND));
		sndInvalid.createFromFile(getResource(parser.getArg(4), EXT_SOUND));
	} else if(parser.is("VIDEO", 1)) {
		//getResource(parser.getArg(1), EXT_VIDEO);
	}
}

void Menu::parseFile(std::string szFileName) {
	Parser parser;
	if(!parser.open(szFileName)) {
		return;
	}

	//Get all the data
	while(parser.parseLine()) {
		//Parse it
		parseLine(parser);
	}
}

std::string Menu::getResource(std::string resource, std::string extension) {
	if(*resource.c_str() == '*') {
		return "menus/common/" + resource.substr(1, std::string::npos) + "." + extension;
	} else {
		return "menus/" + name + "/" + resource + "." + extension;
	}
}

//INTRO
MenuIntro::MenuIntro() : Menu("intro") {
	timer = FPS / 2;
	state = 0;
}

MenuIntro::~MenuIntro() {
}

void MenuIntro::think() {
	Menu::think();

	if(timer == FPS / 2) {
		sfx.play();
	}

	if(input(INPUT_A) && !timer) {
		timer = FPS / 2;
		state++;
	}

	if(timer) {
		timer--;
		if(!timer && state % 2 == 1) {
			timer = FPS / 2;
			state++;
			if(state == 6 && graphics::shader_support) {
				timer = 0;
				setMenu(MENU_TITLE);
			}
			if(state == 8) {
				timer = 0;
				setMenu(MENU_TITLE);
			}
		}
	}
}

void MenuIntro::draw() {
	//Draw our own fade
	float _alpha = timer / (float)(FPS / 2);
	if(state % 2 == 0) {
		_alpha = 1.0 - _alpha;
	}
	graphics::setColor(255, 255, 255, _alpha);
	if(state < 2) {
		graphics::setColor(255, 255, 255, _alpha);
		instructions.draw(0, 0);
	} else if(state < 4) {
		graphics::setColor(255, 255, 255, _alpha);
		disclaimer_en.draw(0, 0);
	} else if(state < 6) {
		graphics::setColor(255, 255, 255, _alpha);
		disclaimer_ja.draw(0, 0);
	} else if(state < 8 && !graphics::shader_support) {
		graphics::setColor(255, 255, 255, _alpha);
		shader_error.draw(0, 0);
	}
}

void MenuIntro::parseLine(Parser& parser) {
	if(parser.is("SFX", 1)) {
		sfx.createFromFile(getResource(parser.getArg(1), EXT_SOUND));
	} else if(parser.is("INSTRUCTIONS", 1)) {
		instructions.createFromFile(getResource(parser.getArg(1), EXT_IMAGE));
	} else if(parser.is("DISCLAIMER", 2)) {
		disclaimer_en.createFromFile(getResource(parser.getArg(1), EXT_IMAGE));
		disclaimer_ja.createFromFile(getResource(parser.getArg(2), EXT_IMAGE));
	} else if(parser.is("SHADER_ERROR", 1)) {
		shader_error.createFromFile(getResource(parser.getArg(1), EXT_IMAGE));
	} else {
		Menu::parseLine(parser);
	}
}

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
#ifndef EMSCRIPTEN
	CHOICE_QUIT,
#endif

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
#ifndef EMSCRIPTEN
	"Quit",
#endif
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

MenuTitle::MenuTitle() : Menu("title") {
	menuX = menuY = 0;
	menuXOffset = 0;
	iR = iG = iB = aR = aG = aB = 255;
	aXOffset = 0;

	choice = choiceLast = choiceTimer = 0;
	submenu = 0;

	nThemes = 0;
	themes = nullptr;
}

MenuTitle::~MenuTitle() {
	delete [] themes;
}

void MenuTitle::init() {
	Menu::init();

	//Parse a random theme
	if(nThemes) {
		//util::roll a weighted random number, favoring the "normal" title
		int i = util::roll(nThemes + 5);
		i -= 5;
		if(i < 0) {
			i = 0;
		}

		parseFile(getResource(themes[i], EXT_SCRIPT));
	}
}

void MenuTitle::think() {
	Menu::think();

	if(choiceTimer) {
		if(choiceTimer == 1 || choiceTimer == -1) {
			choiceTimer = 0;
		} else {
			choiceTimer /= 2;
		}
	}

	uint16_t up = INPUT_UP;
	uint16_t down = INPUT_DOWN;

	if(menuXOffset < 0) {
		up |= INPUT_RIGHT;
		down |= INPUT_LEFT;
	} else {
		up |= INPUT_LEFT;
		down |= INPUT_RIGHT;
	}

	if(input(up)) {
		sndMenu.play();

		choiceTimer = aXOffset;
		choiceLast = choice;
		if(choice == 0) {
			choice = menuChoicesMax[submenu] - 1;
		} else {
			choice--;
		}
	} else if(input(down)) {
		sndMenu.play();

		choiceTimer = aXOffset;
		choiceLast = choice;
		if(choice >= menuChoicesMax[submenu] - 1) {
			choice = 0;
		} else {
			choice++;
		}
	} else if(input(INPUT_A)) {
		//Enter the new menu
		switch(submenu) {
		case TM_MAIN:
			switch(choice) {
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
				setMenu(MENU_SELECT);
				break;
			default:
				//if(sndSelect) sndSelect->play();
				//setMenu(MENU_SELECT);
				sndInvalid.play();
				break;
#ifndef NO_NETWORK
			case CHOICE_NETPLAY:
				sndSelect.play();
				setMenu(MENU_NETPLAY);
				break;
#endif
			case CHOICE_OPTIONS:
				sndSelect.play();
				setMenu(MENU_OPTIONS);
				break;
#ifndef EMSCRIPTEN
			case CHOICE_QUIT:
				setMenu(MENU_QUIT);
				break;
#endif
			}
			break;

		case TM_VERSUS:
			switch(choice) {
			case CHOICE_VS_PLR:
				sndSelect.play();
				FIGHT->gametype = GAMETYPE_VERSUS;
				setMenu(MENU_SELECT);
				break;
			default:
				//if(sndSelect) sndSelect->play();
				//setMenu(MENU_SELECT);
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
	} else if(input(INPUT_B)) {
		if(submenu == TM_MAIN) {
			//Quit
#ifndef EMSCRIPTEN
			setMenu(MENU_QUIT);
#endif
		} else if(submenu == TM_VERSUS) {
			//Return
			sndBack.play();
			choice = CHOICE_VERSUS;
			choiceLast = choice;
			choiceTimer = 0;
			submenu = TM_MAIN;
		}
	}
}

void MenuTitle::reset() {
	Menu::reset();
}

void MenuTitle::draw() {
	Menu::draw();

	if(menuFont.exists()) {
		for(int i = 0; i < menuChoicesMax[submenu]; i++) {
			int gray = 2;
			if(submenu == TM_MAIN) {
				switch(i) {
				case CHOICE_VERSUS:
				case CHOICE_TRAINING:
				case CHOICE_OPTIONS:
#ifndef NO_NETWORK
				case CHOICE_NETPLAY:
#endif
#ifndef EMSCRIPTEN
				case CHOICE_QUIT:
#endif
					gray = 1;
					break;
				}
			} else if(submenu == TM_VERSUS) {
				switch(i) {
				case CHOICE_VS_PLR:
				case CHOICE_VS_RETURN:
					gray = 1;
					break;
				}
			}

			if(i == choice) {
				menuFont.drawText(menuX + i * menuXOffset + (aXOffset - choiceTimer), menuY + menuFont.img.h * i, menuChoices[submenu][i], aR / gray, aG / gray, aB / gray, 255);
			} else if(i == choiceLast) {
				menuFont.drawText(menuX + i * menuXOffset + choiceTimer, menuY + menuFont.img.h * i, menuChoices[submenu][i], iR / gray, iG / gray, iB / gray, 255);
			} else {
				menuFont.drawText(menuX + i * menuXOffset, menuY + menuFont.img.h * i, menuChoices[submenu][i], iR / gray, iG / gray, iB / gray, 255);
			}
		}
	}
}

void MenuTitle::parseLine(Parser& parser) {
	int argc = parser.getArgC();
	if(parser.is("MENU", 3)) {
		//Font
		menuFont.createFromFile(getResource(parser.getArg(1), EXT_FONT));

		menuX = parser.getArgInt(2);
		menuY = parser.getArgInt(3);

		if(argc > 4) {
			menuXOffset = parser.getArgInt(4);
		}
	} else if(parser.is("INACTIVE", 3)) {
		iR = parser.getArgInt(1);
		iG = parser.getArgInt(2);
		iB = parser.getArgInt(3);
	} else if(parser.is("ACTIVE", 3)) {
		aR = parser.getArgInt(1);
		aG = parser.getArgInt(2);
		aB = parser.getArgInt(3);

		if(argc >= 4) {
			aXOffset = parser.getArgInt(4);
		}
	} else if(parser.is("THEMES", 1)) {
		nThemes = 0;
		themes = new std::string[parser.getArgInt(1)];
	} else if(parser.is("THEME", 1)) {
		themes[nThemes++] = parser.getArg(1);
	} else {
		Menu::parseLine(parser);
	}
}

//CHARACTER SELECT
MenuSelect::MenuSelect() : Menu("select") {
	width = height = 0;
	gWidth = gHeight = 0;
	gui = nullptr;
	grid = nullptr;
	gridFighters = nullptr;
	gridC = 0;

	curData = nullptr;

	state = 0;

	cursor_stage = 0;
	cursor_stage_offset = 0;
}

MenuSelect::~MenuSelect() {
	delete gui;
	delete [] curData;
	delete [] grid;
	delete [] gridFighters;
}

void MenuSelect::init() {
	Menu::init();
}

void MenuSelect::think() {
	Menu::think();

	//Visual effects animations

	for(int i = 0; i < 2; i++) {
		if(cursors[i].timerPortrait) {
			if(cursors[i].timerPortrait == 1) {
				cursors[i].timerPortrait = 0;
			} else {
				cursors[i].timerPortrait *= 0.8;
			}
		}
	}

	//Move cursor
	if(state < 2) {
		for(int cur = 0; cur < 2; cur++) {
			uint16_t input = madotsuki.frameInput;
			if(cur == 1) {
				input = poniko.frameInput;
			}
			if(FIGHT->gametype == GAMETYPE_TRAINING) {
				cur = state;
			}
			int group = cursors[cur].getGroup(width, gWidth, gHeight);

			if(!cursors[cur].locked) {
				if(input & INPUT_DIRMASK) {
					//Old cursor pos SET
					cursors[cur].posOld = cursors[cur].pos;
					cursors[cur].timerPortrait = PORTRAIT_FADE;
				}

				if(input & INPUT_LEFT) {
					sndMenu.play();

					do {
						if(cursors[cur].pos % width == 0) {
							cursors[cur].pos += width - 1;
						} else {
							cursors[cur].pos --;
						}
					} while(grid[cursors[cur].pos].x == -1);

					//Check cell limits
					if(cursors[cur].pos >= gridC) {
						cursors[cur].pos += gridC % width - (cursors[cur].pos % width) - 1;
					}
				} else if(input & INPUT_RIGHT) {
					sndMenu.play();

					do {
						if(cursors[cur].pos % width == width - 1) {
							cursors[cur].pos -= width - 1;
						} else {
							cursors[cur].pos++;
						}
					} while(grid[cursors[cur].pos].x == -1);

					//Check cell limits
					if(cursors[cur].pos >= gridC) {
						cursors[cur].pos -= cursors[cur].pos % width;
					}
				} else if(input & INPUT_UP) {
					sndMenu.play();

					do {
						if(cursors[cur].pos / width == 0) {
							cursors[cur].pos += width * (height - 1);
						} else {
							cursors[cur].pos -= width;
						}
					} while(grid[cursors[cur].pos].x == -1);

					//Check cell limits
					if(cursors[cur].pos >= gridC) {
						if(cursors[cur].pos % width < gridC % width) {
							cursors[cur].pos -= width * (gridC / width - 1);
						} else {
							cursors[cur].pos -= width * (gridC / width);
						}
					}
				} else if(input & INPUT_DOWN) {
					sndMenu.play();

					do {
						if(cursors[cur].pos / width == height - 1) {
							cursors[cur].pos -= width * (height - 1);
						} else {
							cursors[cur].pos += width;
						}
					} while(grid[cursors[cur].pos].x == -1);

					//Check cell limits
					if(cursors[cur].pos >= gridC) {
						cursors[cur].pos = cursors[cur].pos % width;
					}
				}

				if(input & INPUT_A) {
					if(gridFighters[cursors[cur].pos] >= 0) {
                        cursors[cur].locked = true;
                        newEffect(cur, group);
                        if(FIGHT->gametype == GAMETYPE_TRAINING) {
                            state++;
                        } else if(cursors[0].locked && cursors[1].locked) {
                            state = 2;
                        }
					} else {
						sndInvalid.play();
					}
				}
			}

			if(input & INPUT_B) {
				if(FIGHT->gametype == GAMETYPE_TRAINING) {
					if(cur == 0) {
						state = 0;

						sndBack.play();
						setMenu(MENU_TITLE);
					} else {
						cur--;
						cursors[cur].locked = false;

						if(curData) {
							group = cursors[cur].getGroup(width, gWidth, gHeight);
							curData[group].sndDeselect.play();
							curData[group].sndSelect.stop();
						}
						state--;
					}
				} else {
					if(cursors[cur].locked) {
						cursors[cur].locked = false;

						if(curData) {
							group = cursors[cur].getGroup(width, gWidth, gHeight);
							curData[group].sndDeselect.play();
							curData[group].sndSelect.stop();
						}
					} else {
						if(!net::connected) {
							sndBack.play();
							setMenu(MENU_TITLE);
						}
					}
				}
			}

			if(FIGHT->gametype == GAMETYPE_TRAINING) {
				break;
			}
		}
	} else {
		if(input(INPUT_LEFT)) {
			sndMenu.play();
			if(cursor_stage % 10 == 0) {
                cursor_stage += 9;
                cursor_stage_offset += 76 * 10;
			} else {
			    cursor_stage--;
			    cursor_stage_offset += -76;
			}
		}

		if(input(INPUT_RIGHT)) {
			sndMenu.play();
			if(cursor_stage % 10 == 9) {
				cursor_stage -= 9;
				cursor_stage_offset += -76 * 10;
			} else {
			    cursor_stage++;
			    cursor_stage_offset += 76;
			}
		}


		if(input(INPUT_UP)) {
			sndMenu.play();
			if(cursor_stage < 10) {
				cursor_stage += 10;
			} else {
			    cursor_stage -= 10;
			}
		}

		if(input(INPUT_DOWN)) {
			sndMenu.play();
			if(cursor_stage >= 11) {
				cursor_stage -= 10;
			} else {
			    cursor_stage += 10;
			}
		}

		if(FIGHT->gametype == GAMETYPE_TRAINING) {
			if(input(INPUT_B)) {
				cursors[1].locked = false;

				if(curData) {
					int group = cursors[1].getGroup(width, gWidth, gHeight);
					curData[group].sndDeselect.play();
					curData[group].sndSelect.stop();
				}
				state--;
			}
		} else {
			if(input(INPUT_B)) {
				cursors[0].locked = false;
				cursors[1].locked = false;

				if(curData) {
					int group = cursors[0].getGroup(width, gWidth, gHeight);
					curData[group].sndDeselect.play();
					curData[group].sndSelect.stop();
					group = cursors[1].getGroup(width, gWidth, gHeight);
					curData[group].sndDeselect.play();
					curData[group].sndSelect.stop();
				}
				state = 0;
			}
		}

		if(input(INPUT_A)) {
			//Start game!
			madotsuki.fighter = &game::fighters[gridFighters[cursors[0].pos]];
			poniko.fighter = &game::fighters[gridFighters[cursors[1].pos]];

            ((MenuVersus*)menus[MENU_VERSUS])->portraits[0] = &madotsuki.fighter->portrait;

            ((MenuVersus*)menus[MENU_VERSUS])->portraits[1] = &poniko.fighter->portrait;

			stage = cursor_stage;
			setMenu(MENU_VERSUS);
			sndSelect.play();
		}
	}
}

void MenuSelect::reset() {
	Menu::reset();

	state = 0;
	for(int i = 0; i < 2; i++) {
		cursors[i].locked = false;
		cursors[i].timer = 0;
		cursors[i].timerPortrait = 0;
		cursors[i].frame = 0;
	}
	cursors[0].pos = cursors[0].posDefault;
	cursors[1].pos = cursors[1].posDefault;
}

void MenuSelect::draw() {
	Menu::draw();

	//Draw portraits first
	if(state >= 1 || FIGHT->gametype == GAMETYPE_VERSUS) {
		if(cursors[1].timerPortrait) {
			if(gridFighters[cursors[1].posOld] >= 0) {
				graphics::setColor(255, 255, 255, (float)(cursors[1].timerPortrait) / PORTRAIT_FADE);
				game::fighters[gridFighters[cursors[1].posOld]].portrait.draw(WINDOW_WIDTH - game::fighters[gridFighters[cursors[1].posOld]].portrait.w + (PORTRAIT_FADE - cursors[1].timerPortrait), 0, true);
			}
		}
		if(gridFighters[cursors[1].pos] >= 0) {
			graphics::setColor(255, 255, 255, (float)(PORTRAIT_FADE - cursors[1].timerPortrait) / PORTRAIT_FADE);
			game::fighters[gridFighters[cursors[1].pos]].portrait.draw(WINDOW_WIDTH - game::fighters[gridFighters[cursors[1].pos]].portrait.w + cursors[1].timerPortrait, 0, true);
		}
	}
	if(cursors[0].timerPortrait) {
		if(gridFighters[cursors[0].posOld] >= 0) {
			graphics::setColor(255, 255, 255, (float)(cursors[0].timerPortrait) / PORTRAIT_FADE);
			game::fighters[gridFighters[cursors[0].posOld]].portrait.draw(0 - (PORTRAIT_FADE - cursors[0].timerPortrait), 0);
		}
	}
	if(gridFighters[cursors[0].pos] >= 0) {
		graphics::setColor(255, 255, 255, (float)(PORTRAIT_FADE - cursors[0].timerPortrait) / PORTRAIT_FADE);
		game::fighters[gridFighters[cursors[0].pos]].portrait.draw(0 - cursors[0].timerPortrait, 0);
	}

	//Now the GUI
	if(gui) {
		gui->draw(false);
	}

	//Draw the select sprites
	for(int i = 0; i < gridC; i++) {
		if(gridFighters[i] >= 0) {
			game::fighters[gridFighters[i]].select.draw(grid[i].x, grid[i].y);
		}
	}

	//Cursor
	//Get the current group
	if(curData) {
		int count = MIN(state, 1);
		if(FIGHT->gametype == GAMETYPE_VERSUS) {
			count = 1;
		}
		for(int i = 0; i <= count; i++) {
			int group = cursors[i].getGroup(width, gWidth, gHeight);;

			if(!cursors[i].locked) {
				graphics::setColor(cursors[i].r, cursors[i].g, cursors[i].b);
			}
			curData[group].img.draw(grid[cursors[i].pos].x + curData[group].off.x, grid[cursors[i].pos].y + curData[group].off.y);

			//Draw the effects
			drawEffect(i, group, grid[cursors[i].pos].x, grid[cursors[i].pos].y);
		}
	}

	//Finally draw the stage selection screen
	if(state >= 2) {
		//Darken background
		glBindTexture(GL_TEXTURE_2D, 0);
		glColor4f(0.0f, 0.0f, 0.0f, 0.7f);
		glBegin(GL_QUADS);
		glVertex3f(0.0f, 0.0f, 0.0f);
		glVertex3f(0.0f, WINDOW_HEIGHT, 0.0f);
		glVertex3f(WINDOW_WIDTH, WINDOW_HEIGHT, 0.0f);
		glVertex3f(WINDOW_WIDTH, 0.0f, 0.0f);
		glEnd();

		//Draw the stage list
		for(int i = 0; i < 20; i++) {
            int x = 38 + (8 + 76) * (i % 10 + 3 - cursor_stage % 10) + cursor_stage_offset;
            int y = 150 + (8 + 50) * (i / 10);
            cursor_stage_offset *= 0.95;
            if(!stages[i].thumbnail.isPlaying())
                stages[i].thumbnail.setPlaying(true);
		    if(cursor_stage == i) {
                graphics::setColor(255, 255, 255, 1.0f);
                stages[i].thumbnail.draw(x, y);
		    } else {
		        graphics::setColor(127, 127, 127, 1.0f);
		        stages[i].thumbnail.draw(x, y);
		    }
		}
	}
}

void MenuSelect::newEffect(int player, int group) {
	if(curData) {
		curData[group].sndSelect.play();
		curData[group].sndDeselect.stop();
	}

	cursors[player].frame = 1;
}

void MenuSelect::drawEffect(int player, int group, int _x, int _y, bool spr) {
	if(cursors[player].frame) {
		float scale = 1.0f;
		float alpha = 1.0f - (cursors[player].frame - 1) / (float)curData[group].frameC;;
		if(curData[group].grow) {
			scale += (cursors[player].frame - 1) * 0.1;
		}
		int x = (cursors[player].frame - 1) % (curData[group].imgSelect.h / 96);

		graphics::setRect(0, x * 96, 96, 96);
		graphics::setColor(255, 255, 255, alpha);

		if(spr) {
			graphics::setScale(scale * 2);
			curData[group].imgSelect.drawSprite(_x - (96 * scale), _y - (96 * scale));
		} else {
			graphics::setScale(scale);
			curData[group].imgSelect.draw(_x - (96 / 2 * scale) + 26 / 2, _y - (96 / 2 * scale) + 29 / 2);
		}

		if(++cursors[player].timer > curData[group].speed) {
			cursors[player].timer = 0;
			if(++cursors[player].frame > curData[group].frameC) {
				cursors[player].frame = 0;
			}
		}
	}
}

void MenuSelect::parseLine(Parser& parser) {
	int argc = parser.getArgC();
	if(parser.is("GRID", 4)) {
		//Make the grids!
		width = parser.getArgInt(1);
		height = parser.getArgInt(2);

		grid = new util::Vector[width * height];
		gridFighters = new int[width * height];

		//Now for the groups...
		gWidth = parser.getArgInt(3);
		gHeight = parser.getArgInt(4);

		int gSize = (width / gWidth) * (height / gHeight);
		curData = new CursorData[gSize];

		//Set the p2 cursor to the width - 1
		cursors[1].pos = width - 1;
		cursors[1].posOld = width - 1;
	} else if(parser.is("CURSOR", 10)) {
		//Load up the cursor

		//Get the group num
		int group = parser.getArgInt(1) - 1;

		//Cursor Image
		curData[group].img.createFromFile(getResource(parser.getArg(2), EXT_IMAGE));

		//X & Y offsets
		curData[group].off.x = parser.getArgInt(3);
		curData[group].off.y = parser.getArgInt(4);

		//Effect stuff
		curData[group].imgSelect.createFromFile(getResource(parser.getArg(5), EXT_IMAGE));
		curData[group].frameC = parser.getArgInt(6);
		curData[group].speed = parser.getArgInt(7);
		curData[group].grow = parser.getArgBool(8, false);

		//Sounds
		curData[group].sndSelect.createFromFile(getResource(parser.getArg(9), EXT_SOUND));
		curData[group].sndDeselect.createFromFile(getResource(parser.getArg(10), EXT_SOUND));
	} else if(parser.is("CHAR", 1)) {
		//Add to the grids
		gridFighters[gridC] = -1;

		//Is it a null?
		if(!strcmp(parser.getArg(1), "null")) {
			grid[gridC].x = -1;
			grid[gridC].y = -1;
			gridC++;
			return;
		}

		if(argc < 3) {
			return;
		}
		if(gridC >= width * height) {
			return;
		}

		//Get fighter
		for(int i = 0; i < FIGHTERS_MAX; i++) {
			if(!game::fighters[i].name.compare(parser.getArg(1))) {
				gridFighters[gridC] = i;
				break;
			}
		}

		//Position
		grid[gridC].x = parser.getArgInt(2);
		grid[gridC].y = parser.getArgInt(3);

		gridC++;
	} else if(parser.is("SELECT", 2)) {
		float x = parser.getArgFloat(2);
		float y = parser.getArgFloat(3);
		char render = RENDER_NORMAL;
		float xvel = 0.0f;
		float yvel = 0.0f;
		bool wrap = false;
		if(argc > 4) {
			const char* szRender = parser.getArg(4);
			if(!strcmp(szRender, "additive")) {
				render = RENDER_ADDITIVE;
			} else if(!strcmp(szRender, "subtractive")) {
				render = RENDER_SUBTRACTIVE;
			} else if(!strcmp(szRender, "multiply")) {
				render = RENDER_MULTIPLY;
			}
			if(argc > 5) {
				xvel = parser.getArgFloat(5);
				if(argc > 6) {
					yvel = parser.getArgFloat(6);
					if(argc > 7) {
						wrap = parser.getArgBool(7, false);
					}
				}
			}
		}

		//Add a new image
		Image imgData;
		imgData.createFromFile(getResource(parser.getArg(1), EXT_IMAGE));
		if(!imgData.exists()) {
			return;
		}
		gui = new MenuImage(imgData, x, y, 1.0f, render, xvel, yvel, wrap, 0);
	} else if(parser.is("STAGES", 1)) {
		//Load the font
		font_stage.createFromFile(getResource(parser.getArg(1), EXT_FONT));
	} else if(parser.is("PLAYER", 6)) {
		//Load the player
		int p = parser.getArgInt(1) - 1;

		//Set the default position of the cursor
		cursors[p].posDefault = parser.getArgInt(3) * width + parser.getArgInt(2);
		cursors[p].pos = cursors[p].posDefault;

		//Colors
		cursors[p].r = parser.getArgInt(4);
		cursors[p].g = parser.getArgInt(5);
		cursors[p].b = parser.getArgInt(6);
	} else {
		Menu::parseLine(parser);
	}
}


//Menu Versus
MenuVersus::MenuVersus() : Menu("versus") {
	//video = nullptr;

	timer1 = timer2 = timer3 = timer4 = timer5 = timer6 = 0;
	timerF = 0.0f;

	portraits[0] = portraits[1] = nullptr;
}

MenuVersus::~MenuVersus() {
}

void MenuVersus::init() {
	Menu::init();
}

void MenuVersus::think() {
	Menu::think();

	if(input(INPUT_A))
        setMenu(MENU_FIGHT);

	if(timer1 > 0) {
		timer1--;
	} else if(timer2 > 0) {
		timer2 *= 0.90;
	} else if(timer3 > 0) {
		timer3--;
	} else if(timer4 > 0) {
		timer4 *= 0.90;
	} else if(timer5 > 0) {
		timer5--;
	} else if(timer6 > 0) {
		timer6--;
		if(!timer6) {
			/*extern util::Vector cameraPos;
			extern util::Vector idealCameraPos;
			cameraPos.x = 0;
			cameraPos.y = 0;
			idealCameraPos.x = 0;
			idealCameraPos.y = 0;*/
			setMenu(MENU_FIGHT);
		}
	}


	if(!timer5) {
		timer2 -= 100;
	}

	if(!timer1) {
		timerF += 0.03f;
	}
}

void MenuVersus::reset() {
	Menu::reset();

	timer1 = FPS * 2;
	timer2 = 1000;
	timer3 = FPS * 1;
	timer4 = 1000;
	timer5 = FPS * 2;
	timer6 = FPS * 1.5;

	timerF = 0.0f;
}

void MenuVersus::draw() {
	Menu::draw();

	if(portraits[1]) {
		graphics::setColor(255, 255, 255, 0.5f);// MIN(1.0f, (FPS * 2 + FPS / 2 - timer6) / 60.0f));
		graphics::setScale(2.0f);
		portraits[1]->draw(WINDOW_WIDTH - portraits[1]->w * 1.5 - timer2 + timerF, -120, true);
		portraits[1]->draw(WINDOW_WIDTH - portraits[1]->w + timer4 - timerF, 0, true);
	}

	if(portraits[0]) {
		graphics::setColor(255, 255, 255, 0.5f);// MIN(1.0f, (FPS * 2 + FPS / 2 - timer6) / 60.0f));
		graphics::setScale(2.0f);
		portraits[0]->draw(portraits[0]->w * -0.5 + timer2 - timerF, -120);
		portraits[0]->draw(0 - timer4 + timerF, 0);
	}
}

void MenuVersus::parseLine(Parser& parser) {
	Menu::parseLine(parser);
}

//Menu Options
MenuOptions::MenuOptions() : Menu("options") {
	cursor = cursorLast = cursorTimer = 0;
	madoPos = 0;
	madoFrame = 1;
	madoDir = 2;
	madoWakeTimer = 0;
	themes = nullptr;
	nThemes = 0;
}

MenuOptions::~MenuOptions() {
	delete [] themes;
}

enum {
	OPTION_DIFFICULTY,
	OPTION_WINS,
	OPTION_TIME,
	OPTION_SFX_VOLUME,
	OPTION_MUS_VOLUME,
	OPTION_VOICE_VOLUME,
	OPTION_EPILEPSY,
	OPTION_CREDITS,

	OPTION_MAX,
};

int optionDifficulty = 3;
int optionWins = 2;
int optionTime = 99;
int optionSfxVolume = 100;
int optionMusVolume = 100;
int optionVoiceVolume = 100;
bool optionEpilepsy = false;

void MenuOptions::think() {
	Menu::think();

	//Cursor stuff
	if(cursorTimer) {
		if(cursorTimer == 1 || cursorTimer == -1) {
			cursorTimer = 0;
		} else {
			cursorTimer /= 2;
		}
	}

	//Move/animate Madotsuki
	if(madoWakeTimer) {
		if(madoWakeTimer == 1) {
			madoWakeTimer = -1;
			setMenu(MENU_TITLE);
		} else {
			madoWakeTimer--;
			if(madoWakeTimer == 36) {
				madoFrame++;
			} else if(madoWakeTimer == 32) {
				madoSfxPinch.play();
				madoFrame++;
			}
		}
	} else {
		bool stopped = false;
		if(madoPos < cursor * 32) {
			//Move down
			madoDir = 2;
			madoPos++;
			if(madoPos % 8 == 0) {
				madoFrame++;
				if(madoFrame >= 4) {
					madoFrame = 0;
				}
				if(madoFrame % 2 == 1) {
					madoSfxStep.play();
				}
			}
		} else if(madoPos > cursor * 32) {
			//Move up
			madoDir = 0;
			madoPos --;
			if(madoPos % 8 == 0) {
				madoFrame++;
				if(madoFrame >= 4) {
					madoFrame = 0;
				}
				if(madoFrame % 2 == 1) {
					madoSfxStep.play();
				}
			}
		} else {
			madoFrame = 1;
			stopped = true;
		}

		if(input(INPUT_UP | INPUT_DOWN)) {
			cursorTimer = aXOffset;
			cursorLast = cursor;
		}

		if(input(INPUT_UP)) {
			sndMenu.play();
			if(cursor) {
				cursor--;
			} else {
				cursor = OPTION_MAX - 1;
			}
		} else if(input(INPUT_DOWN)) {
			sndMenu.play();
			if(cursor < OPTION_MAX - 1) {
				cursor++;
			} else {
				cursor = 0;
			}
		}
		if(stopped) {
			if(input(INPUT_LEFT)) {
				madoDir = 3;
			} else if(input(INPUT_RIGHT)) {
				madoDir = 1;
			}
		}

		//Change option
		if(input(INPUT_LEFT) || input(INPUT_RIGHT)) {
			if(cursor != OPTION_VOICE_VOLUME) {
				sndMenu.play();
			}

			switch(cursor) {
			case OPTION_DIFFICULTY:
				if(input(INPUT_LEFT)) {
					if(optionDifficulty > 1) {
						optionDifficulty--;
					}
				} else {
					if(optionDifficulty < 5) {
						optionDifficulty++;
					}
				}
				break;

			case OPTION_WINS:
				if(input(INPUT_LEFT)) {
					if(optionWins > 1) {
						optionWins--;
					}
				} else {
					if(optionWins < 3) {
						optionWins++;
					}
				}
				break;

			case OPTION_TIME:
				if(input(INPUT_LEFT)) {
					if(optionTime == 0) {
						optionTime = 99;
					} else if(optionTime == 99) {
						optionTime = 60;
					}
				} else {
					if(optionTime == 60) {
						optionTime = 99;
					} else if(optionTime == 99) {
						optionTime = 0;
					}
				}
				break;

			case OPTION_SFX_VOLUME:
				if(input(INPUT_LEFT)) {
					if(optionSfxVolume > 0) {
						optionSfxVolume -= 10;
					}
				} else {
					if(optionSfxVolume < 100) {
						optionSfxVolume += 10;
					}
				}
				break;

			case OPTION_MUS_VOLUME:
				if(input(INPUT_LEFT)) {
					if(optionMusVolume > 0) {
						optionMusVolume -= 10;
					}
				} else {
					if(optionMusVolume < 100) {
						optionMusVolume += 10;
					}
				}
#ifdef EMSCRIPTEN
				Mix_VolumeMusic(optionMusVolume * MIX_MAX_VOLUME / 100);
#endif
				break;

			case OPTION_VOICE_VOLUME:
				if(input(INPUT_LEFT)) {
					if(optionVoiceVolume > 0) {
						optionVoiceVolume -= 10;
					}
					madotsuki.speaker.play(&dame);
				} else {
					if(optionVoiceVolume < 100) {
						optionVoiceVolume += 10;
					}
					madotsuki.speaker.play(&muri);
				}
				break;

			case OPTION_EPILEPSY:
				if(input(INPUT_LEFT | INPUT_RIGHT)) {
					optionEpilepsy = !optionEpilepsy;
				}
				break;
			}
		}

		if(input(INPUT_A)) {
			if(cursor == OPTION_CREDITS) {
				sndSelect.play();
				setMenu(MENU_CREDITS);
			}
		}

		//Pinch thyself awake, Madotsuki
		if(input(INPUT_B)) {
			madoDir = 4;
			madoFrame = 0;
			madoWakeTimer = 40;
		}
	}
}

void MenuOptions::reset() {
	Menu::reset();

	cursor = cursorLast = cursorTimer = 0;
	madoPos = 0;
	madoFrame = 1;
	madoDir = 2;
	madoWakeTimer = 0;
}

void MenuOptions::draw() {
	Menu::draw();

	//Draw the menu options
	if(menuFont.exists()) {
		for(int i = 0; i < OPTION_MAX; i++) {
			char buff[80];
			switch(i) {
			case OPTION_DIFFICULTY:
				sprintf(buff, "Difficulty:\t%d", optionDifficulty);
				break;

			case OPTION_WINS:
				sprintf(buff, "Wins:\t%d", optionWins);
				break;

			case OPTION_TIME:
				if(optionTime) {
					sprintf(buff, "Time:\t%d", optionTime);
				} else {
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
				if(optionEpilepsy) {
					strcpy(buff, "Photosensitivity Mode:\tOn");
				} else {
					strcpy(buff, "Photosensitivity Mode:\tOff");
				}
				break;

			case OPTION_CREDITS:
				strcpy(buff, "Credits");
				break;
			}

			if(i == cursor) {
				menuFont.drawText(64 + (aXOffset - cursorTimer), 64 + i * 32, buff, aR, aG, aB);
			} else if(i == cursorLast) {
				menuFont.drawText(64 + cursorTimer, 64 + i * 32, buff, iR, iG, iB);
			} else {
				menuFont.drawText(64, 64 + i * 32, buff, iR, iG, iB);
			}
		}
	}

	int xoff = madoFrame * 16;
	if(madoFrame == 3) {
		xoff = 16;
	}
	graphics::setRect(xoff, madoDir * 32, 16, 32);
	madoImg.draw(32, 64 + madoPos);
}

void MenuOptions::init() {
	Menu::init();

	//Parse a random theme
	if(nThemes) {
		parseFile(getResource(themes[util::roll(nThemes)], EXT_SCRIPT));
	}
}

void MenuOptions::parseLine(Parser& parser) {
	int argc = parser.getArgC();
	if(parser.is("FONT", 1)) {
		//The font
		menuFont.createFromFile(getResource(parser.getArg(1), EXT_FONT));
	} else if(parser.is("MADOTSUKI", 3)) {
		//Madotsuki sprites/sounds
		madoImg.createFromFile(getResource(parser.getArg(1), EXT_IMAGE));
		madoSfxStep.createFromFile(getResource(parser.getArg(2), EXT_SOUND));
		madoSfxPinch.createFromFile(getResource(parser.getArg(3), EXT_SOUND));
	} else if(parser.is("INACTIVE", 3)) {
		iR = parser.getArgInt(1);
		iG = parser.getArgInt(2);
		iB = parser.getArgInt(3);
	} else if(parser.is("ACTIVE", 3)) {
		aR = parser.getArgInt(1);
		aG = parser.getArgInt(2);
		aB = parser.getArgInt(3);

		if(argc > 4) {
			aXOffset = parser.getArgInt(4);
		}
	} else if(parser.is("THEMES", 1)) {
		nThemes = 0;
		themes = new std::string[parser.getArgInt(1)];
	} else if(parser.is("THEME", 1)) {
		themes[nThemes++] = parser.getArg(1);
	} else if(parser.is("VOICES", 2)) {
		dame.createFromFile(getResource(parser.getArg(1), EXT_SOUND));
		muri.createFromFile(getResource(parser.getArg(2), EXT_SOUND));
	} else {
		Menu::parseLine(parser);
	}
}


//FIGHT
MenuMeter::MenuMeter() {
}

MenuMeter::~MenuMeter() {
}

void MenuMeter::draw(float pct, bool mirror, bool flip) {
	if(pct > 0) {
		if(flip) {
			graphics::setRect(0, 0, img.w * pct, img.h);
		} else {
			graphics::setRect(img.w * (1-pct), 0, img.w * pct + 1, img.h);
		}

		if(mirror) {
			if(flip) {
				img.draw(WINDOW_WIDTH - img.w - pos.x + img.w * (1 - pct), pos.y, true);
			} else {
				//Hack
				if(pct == 1.0f) {
					img.draw(WINDOW_WIDTH - img.w - pos.x, pos.y, true);
				} else {
					img.draw(WINDOW_WIDTH - img.w - pos.x + 1, pos.y, true);
				}
			}
		} else {
			if(flip) {
				img.draw(pos.x, pos.y);
			} else {
				img.draw(pos.x + img.w * (1 - pct), pos.y);
			}
		}
	}
}

MenuFight::MenuFight() : Menu("fight") {
	gametype = GAMETYPE_TRAINING;
	reset();
}

MenuFight::~MenuFight() {
}

void MenuFight::init() {
	Menu::init();
}

void MenuFight::parseLine(Parser& parser) {
	if(parser.is("HUD", 1)) {
		hud.createFromFile(getResource(parser.getArg(1), EXT_IMAGE));
	} else if(parser.is("HUD_TAG", 1)) {
		hud_tag.createFromFile(getResource(parser.getArg(1), EXT_IMAGE));
	} else if(parser.is("PORTRAITS", 1)) {
		portraits.createFromFile(getResource(parser.getArg(1), EXT_IMAGE));
	} else if(parser.is("PORTRAITS_TAG", 1)) {
		portraits_tag.createFromFile(getResource(parser.getArg(1), EXT_IMAGE));
	} else if(parser.is("TIMER", 2)) {
		timer.createFromFile(getResource(parser.getArg(1), EXT_IMAGE));
		timer_font.createFromFile(getResource(parser.getArg(2), EXT_FONT));
	} else if(parser.is("SHINE", 1)) {
		shine.createFromFile(getResource(parser.getArg(1), EXT_IMAGE));
	} else if(parser.is("SHINE_TAG", 1)) {
		shine_tag.createFromFile(getResource(parser.getArg(1), EXT_IMAGE));
	} else if(parser.is("HP", 3)) {
		meterHp.img.createFromFile(getResource(parser.getArg(1), EXT_IMAGE));

		meterHp.pos.x = parser.getArgInt(2);
		meterHp.pos.y = parser.getArgInt(3);
	} else if(parser.is("SUPER", 3)) {
		meterSuper.img.createFromFile(getResource(parser.getArg(1), EXT_IMAGE));

		meterSuper.pos.x = parser.getArgInt(2);
		meterSuper.pos.y = parser.getArgInt(3);
	} else if(parser.is("TAG", 3)) {
		meterTag.img.createFromFile(getResource(parser.getArg(1), EXT_IMAGE));

		meterTag.pos.x = parser.getArgInt(2);
		meterTag.pos.y = parser.getArgInt(3);
	} else if(parser.is("STUN", 3)) {
		meterStun.img.createFromFile(getResource(parser.getArg(1), EXT_IMAGE));

		meterStun.pos.x = parser.getArgInt(2);
		meterStun.pos.y = parser.getArgInt(3);
	} else if(parser.is("GUARD", 3)) {
		meterGuard.img.createFromFile(getResource(parser.getArg(1), EXT_IMAGE));

		meterGuard.pos.x = parser.getArgInt(2);
		meterGuard.pos.y = parser.getArgInt(3);
	} else if(parser.is("DPM", 3)) {
		meterDpm.img.createFromFile(getResource(parser.getArg(1), EXT_IMAGE));

		meterDpm.pos.x = parser.getArgInt(2);
		meterDpm.pos.y = parser.getArgInt(3);
	} else if(parser.is("STATIC", 2)) {
		staticImg.createFromFile(getResource(parser.getArg(1), EXT_IMAGE));
		staticSnd.createFromFile(getResource(parser.getArg(2), EXT_SOUND));
	} else if(parser.is("FADE", 2)) {
		fadeinSnd.createFromFile(getResource(parser.getArg(1), EXT_SOUND));
		fadeoutSnd.createFromFile(getResource(parser.getArg(2), EXT_SOUND));
	} else if(parser.is("ROUND_SPLASH", 5)) {
		for(int i = 0; i < 5; i++) {
			round_splash[i].createFromFile(getResource(parser.getArg(i+1), EXT_IMAGE));
		}
	} else if(parser.is("ROUND_HUD", 7)) {
		for(int i = 0; i < 5; i++) {
			round_hud[i].createFromFile(getResource(parser.getArg(i+1), EXT_IMAGE));
		}
		x_round_hud = parser.getArgInt(6);
		y_round_hud = parser.getArgInt(7);
	} else if(parser.is("KO", 3)) {
		for(int i = 0; i < 3; i++) {
			ko[i].createFromFile(getResource(parser.getArg(i+1), EXT_IMAGE));
		}
	} else if(parser.is("PORTRAIT_POS", 2)) {
		portraitPos.x = parser.getArgInt(1);
		portraitPos.y = parser.getArgInt(2);
	} else if(parser.is("COMBO", 3)) {
		combo.createFromFile(getResource(parser.getArg(1), EXT_FONT));
		comboLeft.createFromFile(getResource(parser.getArg(2), EXT_IMAGE));
		comboRight.createFromFile(getResource(parser.getArg(3), EXT_IMAGE));
	} else if(parser.is("WIN", 3)) {
		win.createFromFile(getResource(parser.getArg(1), EXT_IMAGE));
		win_font.createFromFile(getResource(parser.getArg(2), EXT_FONT));
		win_bgm.createFromFile("", getResource(parser.getArg(3), EXT_MUSIC));
	} else if(parser.is("WIN_ORBS", 5)) {
		orb_null.createFromFile(getResource(parser.getArg(1), EXT_IMAGE));
		orb_win.createFromFile(getResource(parser.getArg(2), EXT_IMAGE));
		orb_draw.createFromFile(getResource(parser.getArg(3), EXT_IMAGE));

		orb_pos.x = parser.getArgInt(4);
		orb_pos.y = parser.getArgInt(5);
	} else {
		Menu::parseLine(parser);
	}
}

void MenuFight::think() {
	Menu::think();

	if(gametype == GAMETYPE_TRAINING) {
		ko_player = 0;
		timer_ko = 0;
		timer_round_in = 0;
		timer_round_out = 0;
		game_timer = 0;

		if(!bgmPlaying) {
			STAGE.bgmPlay();
			bgmPlaying = true;
		}
	}

	if(winner) {
		if(input(INPUT_B)) {
			setMenu(MENU_SELECT);
		} else if(input(INPUT_A)) {
			reset();
		}
	} else {
		//Decrement timer
		if(game_timer && !timer_ko && !timer_round_in && !timer_round_out) {
			if(!--game_timer) {
				timer_ko = 1 * FPS;
				ko_type = 1;
				if(madotsuki.hp == poniko.hp) {
					ko_player = 3;
				} else if(madotsuki.hp < poniko.hp) {
					ko_player = 1;
				} else {
					ko_player = 2;
				}

				if(madotsuki.flags & F_ON_GROUND && madotsuki.isDashing()) {
					if(madotsuki.inStandardState(game::STATE_DASH_FORWARD)) {
						madotsuki.setStandardState(game::STATE_DASH_FORWARD_END);
					}
					if(madotsuki.inStandardState(game::STATE_DASH_BACK)) {
						madotsuki.setStandardState(game::STATE_DASH_BACK_END);
					}
				}
				if(poniko.flags & F_ON_GROUND && poniko.isDashing()) {
					if(poniko.inStandardState(game::STATE_DASH_FORWARD)) {
						poniko.setStandardState(game::STATE_DASH_FORWARD_END);
					}
					if(poniko.inStandardState(game::STATE_DASH_BACK)) {
						poniko.setStandardState(game::STATE_DASH_BACK_END);
					}
				}
			}
		}

		//Combo counters
		//LEFT
		if(madotsuki.comboCounter > 1 && !ko_player) {
			if(comboLeftLast < madotsuki.comboCounter) {
				comboLeftTimer = FPS;
			} else if(comboLeftLast > madotsuki.comboCounter) {
				comboLeftOff = 0;
				comboLeftTimer = FPS;
			}
			comboLeftLast = madotsuki.comboCounter;
		}

		if(comboLeftTimer) {
			comboLeftTimer--;
			if(comboLeftOff < comboLeft.w) {
				comboLeftOff += 16;
			}
			if(comboLeftOff > comboLeft.w) {
				comboLeftOff = comboLeft.w;
			}
		} else if(comboLeftOff && (madotsuki.comboCounter < 2 || ko_player)) {
			if(comboLeftOff > 0) {
				comboLeftOff -= 16;
			}
			if(comboLeftOff <= 0) {
				comboLeftOff = 0;
				comboLeftLast = 0;
			}
		}

		//RIGHT
		if(poniko.comboCounter > 1 && !ko_player) {
			if(comboRightLast < poniko.comboCounter) {
				comboRightTimer = FPS;
			} else if(comboRightLast > poniko.comboCounter) {
				comboRightOff = 0;
				comboRightTimer = FPS;
			}
			comboRightLast = poniko.comboCounter;
		}

		if(comboRightTimer) {
			comboRightTimer--;
			if(comboRightOff < comboRight.w) {
				comboRightOff += 16;
			}
			if(comboRightOff > comboRight.w) {
				comboRightOff = comboRight.w;
			}
		} else if(comboRightOff && (poniko.comboCounter < 2 || ko_player)) {
			if(comboRightOff > 0) {
				comboRightOff -= 16;
			}
			if(comboRightOff <= 0) {
				comboRightOff = 0;
				comboRightLast = 0;
			}
		}


		//ROUND INTROS
		if(!optionEpilepsy && (timer_round_out || timer_round_in)) {
			if(!timer_flash && !util::roll(64)) {
				staticSnd.play();
				timer_flash = 5;
				staticImg.draw(0, 0);
			}
		}
		if(timer_round_in == (int)(4.0 * FPS)) {
			madotsuki.reset();
			poniko.reset();
			extern util::Vector cameraPos;
			extern util::Vector idealCameraPos;
			cameraPos.x = 0;
			cameraPos.y = 0;
			idealCameraPos.x = 0;
			idealCameraPos.y = 0;
			fadeinSnd.play();
		}
		if(timer_round_out == (int)(1.5 * FPS)) {
			fadeoutSnd.play();
		}

		if(timer_round_in == (int)(1.4 * FPS) && !bgmPlaying) {
			STAGE.bgmPlay();
			bgmPlaying = true;
		}

		if(timer_round_in) {
			timer_round_in--;
		}
		if(timer_round_out) {
			timer_round_out--;
			if(timer_round_out == (int)(3.8 * FPS)) {
				if(ko_player == 2) {
					if(!(poniko.flags & F_DEAD)) {
						poniko.setStandardState(game::STATE_DEFEAT);
					}
					madotsuki.setStandardState(game::STATE_VICTORY);
					win_types[0][wins[0]++] = 0;
				} else if(ko_player == 1) {
					if(!(madotsuki.flags & F_DEAD)) {
						madotsuki.setStandardState(game::STATE_DEFEAT);
					}
					poniko.setStandardState(game::STATE_VICTORY);
					win_types[1][wins[1]++] = 0;
				} else if(ko_player == 3) {
					if(ko_type != 2) {
						timer_round_out = 0;
						timer_ko = 1 * FPS;
						ko_type = 2;
					} else {
						if(!(madotsuki.flags & F_DEAD)) {
							madotsuki.setStandardState(game::STATE_DEFEAT);
						}
						if(!(poniko.flags & F_DEAD)) {
							poniko.setStandardState(game::STATE_DEFEAT);
						}
						win_types[0][wins[0]++] = 1;
						win_types[1][wins[1]++] = 1;
						ko_type = 0;
					}
				}
			} else if(!timer_round_out) {
				//See if someone's won
				if(wins[0] >= optionWins || wins[1] >= optionWins) {
					//Count up the wins
					int wins_p1 = 0;
					int wins_p2 = 0;
					for(int i = 0; i < wins[0]; i++)
						if(win_types[0][i] == 0) {
							wins_p1++;
						}
					for(int i = 0; i < wins[1]; i++)
						if(win_types[1][i] == 0) {
							wins_p2++;
						}

					if(wins_p1 > wins_p2) {
						winner = 1;
					} else if(wins_p1 < wins_p2) {
						winner = 2;
					} else {
						winner = 3;
					}
				}

				if(winner) {
					win_bgm.play();
				}
				timer_round_in = 4.0 * FPS;
				ko_player = 0;
				game_timer = optionTime * FPS - 1;
				if(game_timer < 0) {
					game_timer = 0;
				}
				round++;
			}
		}
		if(timer_ko) {
			timer_ko--;
		}

		if(ko_player && !timer_ko && !timer_round_out) {
			//Make sure everyone's still
			bool _condition = false;
			if(ko_type == 0) {
				if(ko_player == 2) {
					_condition = madotsuki.isIdle() && (madotsuki.flags & F_ON_GROUND) &&
					             (poniko.inStandardState(game::STATE_PRONE) || poniko.inStandardState(game::STATE_ON_BACK));
				} else if(ko_player == 1) {
					_condition = poniko.isIdle() && (poniko.flags & F_ON_GROUND) &&
					             (madotsuki.inStandardState(game::STATE_PRONE) || madotsuki.inStandardState(game::STATE_ON_BACK));
				}
			} else if(ko_type == 1) {
				_condition = poniko.isIdle() && (poniko.flags & F_ON_GROUND) && madotsuki.isIdle() && (madotsuki.flags & F_ON_GROUND);
			} else if(ko_type == 2) {
				_condition = true;
			}
			if(_condition) {
				timer_round_out = 4 * FPS;
			}
		}
	}
}

void MenuFight::draw() {
	Menu::draw();

	if(winner) {
		win.draw(0, 0);

		char _b_sz[256];
		if(winner == 3) {
			strcpy(_b_sz, "Draw!");
		} else {
			sprintf(_b_sz, "Player %d wins!", winner);
		}
		win_font.drawText(32, FLIP(32), _b_sz);
	} else {
		hud.draw(0, 0);
		hud.draw(WINDOW_WIDTH - hud.w, 0, true);

		//DRAW METERS

		meterHp.draw(madotsuki.hp / (float)madotsuki.getMaxHp(), false, false);
		meterHp.draw(poniko.hp / (float)poniko.getMaxHp(), true, false);

		meterSuper.draw(madotsuki.super / (float)SUPER_MAX, false, false);
		meterSuper.draw(poniko.super / (float)SUPER_MAX, true, false);

		//meterTag.draw(1, false, false);
		//meterTag.draw(1, true, false);

		meterStun.draw(1, false, false);
		meterStun.draw(1, true, false);

		meterGuard.draw(1, false, true);
		meterGuard.draw(1, true, true);

		meterDpm.draw(1, false, false);
		meterDpm.draw(1, true, false);

		portraits.draw(0, 0);
		portraits.draw(WINDOW_WIDTH - shine.w, 0, true);

		timer.draw(0, 0);

		if(FIGHT->gametype != GAMETYPE_TRAINING) {
			//Round orbs
			for(int i = 0; i < optionWins; i++) {
				int x = orb_pos.x - i * 18;
				if(i < wins[0]) {
					if(win_types[0][i]) {
						orb_draw.draw(x, orb_pos.y);
					} else {
						orb_win.draw(x, orb_pos.y);
					}
				} else {
					orb_null.draw(x, orb_pos.y);
				}

				if(i < wins[1]) {
					if(win_types[0][i]) {
						orb_draw.draw(WINDOW_WIDTH - x - 18, orb_pos.y);
					} else {
						orb_win.draw(WINDOW_WIDTH - x - 18, orb_pos.y);
					}
				} else {
					orb_null.draw(WINDOW_WIDTH - x - 18, orb_pos.y);
				}
			}

			//Draw timer
			char b_timer_text[8];
			if(game_timer) {
				sprintf(b_timer_text, "%02d", (game_timer / FPS) + 1);
			} else {
				strcpy(b_timer_text, "00");
			}
			int w_timer_text = timer_font.getTextWidth(b_timer_text);
			timer_font.drawText((WINDOW_WIDTH - w_timer_text) / 2, 30, b_timer_text);
		}

		shine.draw(0, 0);
		shine.draw(WINDOW_WIDTH - shine.w, 0, true);

		round_hud[round].draw(x_round_hud, y_round_hud);

		//Draw combo counters
		//LEFT
		if(comboLeftOff) {
			comboLeft.draw(comboLeftOff - comboLeft.w, 131);
			char buff[8];
			sprintf(buff, "%d", comboLeftLast);
			int w = combo.getTextWidth(buff);
			combo.drawText(comboLeftOff - w / 2 - 100, 131 + 35, buff);
		}

		//RIGHT
		if(comboRightOff) {
			comboRight.draw(WINDOW_WIDTH - comboRightOff, 131);
			char buff[8];
			sprintf(buff, "%d", comboRightLast);
			int w = combo.getTextWidth(buff);
			combo.drawText(WINDOW_WIDTH - comboRightOff - w / 2 + 100, 131 + 35, buff);
		}

		//Draw character portraits
		extern int madotsuki_palette;
		extern int poniko_palette;
		if(graphics::shader_support) {
			graphics::setPalette(madotsuki.fighter->palettes[madotsuki_palette], 1.0f, 0.0f, 0.0f, 0.0f, 0.0f);
		}
		madotsuki.fighter->portrait_ui.draw(portraitPos.x, portraitPos.y);
		if(graphics::shader_support) {
			graphics::setPalette(poniko.fighter->palettes[poniko_palette], 1.0f, 0.0f, 0.0f, 0.0f, 0.0f);
		} else {
			graphics::setColor(150, 150, 150, 1.0);
		}
		poniko.fighter->portrait_ui.draw(WINDOW_WIDTH - portraitPos.x - poniko.fighter->portrait_ui.w, portraitPos.y, true);
		if(graphics::shader_support) {
			glUseProgram(0);
		}


		//Draw round transitions
		if(timer_flash) {
			graphics::setColor(180, 120, 190, timer_flash / 5.0f);
			staticImg.draw(-util::roll(WINDOW_WIDTH), -util::roll(WINDOW_HEIGHT));
			timer_flash--;
		}

		if(timer_round_out) {
			float alpha = 0.0f;
			if(timer_round_out < 0.5 * FPS) {
				alpha = 1.0f;
			} else if(timer_round_out < 1.5 * FPS) {
				alpha = 1.0 - ((timer_round_out - 0.5 * FPS) / (1.0 * FPS));
			}
			graphics::setColor(180, 120, 190, alpha);
			staticImg.draw(-util::roll(WINDOW_WIDTH), -util::roll(WINDOW_HEIGHT));
		}

		if(timer_round_in) {
			if(timer_round_in > 1.0 * FPS) {
				graphics::setColor(180, 120, 190, ((timer_round_in - 1.0 * FPS) / (3.0 * FPS)));
				staticImg.draw(-util::roll(WINDOW_WIDTH), -util::roll(WINDOW_HEIGHT));
			}

			if(timer_round_in <= 1.4 * FPS) {
				if(timer_round_in > 1.3 * FPS) {
					float scalar = (timer_round_in - 1.3 * FPS) / (0.1 * FPS) + 1.0;
					graphics::setScale(scalar);
					round_splash[round].draw(WINDOW_WIDTH / 2 - round_splash[round].w * scalar / 2 - util::roll(10, 30), WINDOW_HEIGHT / 2 - round_splash[round].h * scalar / 2 - util::roll(10, 30));
				} else if(timer_round_in < 0.1 * FPS) {
					float scalar = timer_round_in / (0.1 * FPS);
					graphics::setColor(255, 255, 255, scalar);
					round_splash[round].draw(WINDOW_WIDTH / 2 - round_splash[round].w / 2, WINDOW_HEIGHT / 2 - round_splash[round].h / 2);
					graphics::setColor(255, 255, 255, scalar);
					scalar = 1.0f - scalar + 1.0f;
					float xscalar = 1 / scalar;
					graphics::setScale(xscalar, scalar);
					round_splash[round].draw(WINDOW_WIDTH / 2 - round_splash[round].w * xscalar / 2 - util::roll(10, 30), WINDOW_HEIGHT / 2 - round_splash[round].h * scalar / 2 - util::roll(10, 30));
				} else {
					round_splash[round].draw(WINDOW_WIDTH / 2 - round_splash[round].w / 2 - util::roll(5, 15), WINDOW_HEIGHT / 2 - round_splash[round].h / 2 - util::roll(5, 15));
				}
			}
		}

		if(timer_ko) {
			if(timer_ko > 0.8 * FPS) {
				float scalar = (timer_ko - 0.8 * FPS) / (0.1 * FPS) + 1.0;
				graphics::setScale(scalar);
				ko[ko_type].draw(WINDOW_WIDTH / 2 - ko[ko_type].w * scalar / 2 - util::roll(10, 30), WINDOW_HEIGHT / 2 - ko[ko_type].h * scalar / 2 - util::roll(10, 30));
			} else if(timer_ko < 0.1 * FPS) {
				float scalar = timer_ko / (0.1 * FPS);
				graphics::setColor(255, 255, 255, scalar);
				ko[ko_type].draw(WINDOW_WIDTH / 2 - ko[ko_type].w / 2, WINDOW_HEIGHT / 2 - ko[ko_type].h / 2);
				graphics::setColor(255, 255, 255, scalar);
				scalar = 1.0f - scalar + 1.0f;
				float xscalar = 1 / scalar;
				graphics::setScale(xscalar, scalar);
				ko[ko_type].draw(WINDOW_WIDTH / 2 - ko[ko_type].w * xscalar / 2 - util::roll(10, 30), WINDOW_HEIGHT / 2 - ko[ko_type].h * scalar / 2 - util::roll(10, 30));
			} else {
				ko[ko_type].draw(WINDOW_WIDTH / 2 - ko[ko_type].w / 2 - util::roll(5, 15), WINDOW_HEIGHT / 2 - ko[ko_type].h / 2 - util::roll(5, 15));
			}
		}
	}
}

void MenuFight::reset() {
	ko_player = 0;
	ko_type = 0;

	timer_flash = 0;
	timer_round_in = 4.0 * FPS;
	timer_round_out = 0;
	timer_ko = 0;

	round = 0;
	wins[0] = 0;
	wins[1] = 0;
	winner = 0;

	comboLeftOff = comboRightOff = 0;
	comboLeftLast = comboRightLast = 0;
	comboLeftTimer = comboRightTimer = 0;

	game_timer = optionTime * FPS - 1;
	if(game_timer < 0) {
		game_timer = 0;
	}

	bgmPlaying = false;
}

void MenuFight::knockout(int player) {
	if(ko_player) {
		return;
	}
	ko_player = player + 1;
	timer_ko = 1 * FPS;
	ko_type = 0;

	if(madotsuki.flags & F_ON_GROUND && madotsuki.isDashing()) {
		if(madotsuki.inStandardState(game::STATE_DASH_FORWARD)) {
			madotsuki.setStandardState(game::STATE_DASH_FORWARD_END);
		}
		if(madotsuki.inStandardState(game::STATE_DASH_BACK)) {
			madotsuki.setStandardState(game::STATE_DASH_BACK_END);
		}
	}
	if(poniko.flags & F_ON_GROUND && poniko.isDashing()) {
		if(poniko.inStandardState(game::STATE_DASH_FORWARD)) {
			poniko.setStandardState(game::STATE_DASH_FORWARD_END);
		}
		if(poniko.inStandardState(game::STATE_DASH_BACK)) {
			poniko.setStandardState(game::STATE_DASH_BACK_END);
		}
	}
}

//NETPLAY
#ifndef NO_NETWORK
#define NET_FLASH_TIME (FPS / 2)
#define NET_FLASH_HOLD_TIME (FPS / 2)
#define NET_FADE_TIME (FPS)
#define NET_SCALE (2)
#define NET_BAR_SIZE 120
MenuNetplay::MenuNetplay() : Menu("netplay") {
	choice = 0;

	flashTimer = 0;
	flashDir = 1;
	drawShake = 0;
	barPos = 0;
	waiting = false;

	port = DEFAULT_PORT;
	ip = DEFAULT_IP;
	updateIp(false);
	updatePort(false);
}

MenuNetplay::~MenuNetplay() {
}

void MenuNetplay::think() {
	Menu::think();

	if(os::frame % 2) {
		drawShake = !drawShake;
	}

	if(!flashDir) {
		switch(mode) {
		case net::MODE_NONE: {
			if(input(INPUT_A)) {
				sndSelect.play();
				if(choice) {
					mode = net::MODE_CLIENT;
				} else {
					mode = net::MODE_SERVER;
				}
				choice = 0;
			} else if(input(INPUT_B)) {
				sndOff.play();
				flashDir = -1;
				flashTimer = NET_FLASH_TIME + NET_FLASH_HOLD_TIME + NET_FADE_TIME / 2;
			}
			if(input(INPUT_UP | INPUT_DOWN)) {
				sndMenu.play();
				choice = !choice;
			}
		}
		break;

		case net::MODE_SERVER: {
			if(waiting) {
				if(input(INPUT_A)) {
					if(net::connected) {
						sndSelect.play();
						FIGHT->gametype = GAMETYPE_VERSUS;
						setMenu(MENU_SELECT);
					}
				} else if(input(INPUT_B)) {
					if(!net::connected) {
						sndBack.play();
						bgm.play();
						waiting = false;
						net::stop();
					}
				}
			} else if(!digit) {
				if(input(INPUT_A)) {
					sndSelect.play();
					if(choice > 0) {
						digit = 1;
					} else {
						waiting = true;
						net::start(0, port);
						bgmWait.play();
					}
				} else if(input(INPUT_B)) {
					sndBack.play();
					mode = net::MODE_NONE;
					choice = 0;
				}
				if(input(INPUT_UP | INPUT_DOWN)) {
					sndMenu.play();
					choice = !choice;
				}
			} else {
				if(input(INPUT_A)) {
					//Save changes
					sndSelect.play();
					updatePort(true);
					digit = 0;
				} else if(input(INPUT_B)) {
					//Discard changes
					sndBack.play();
					updatePort(false);
					digit = 0;
				}
				if(input(INPUT_DIRMASK)) {
					sndMenu.play();
				}
				if(input(INPUT_UP)) {
					if(++portStr[digit-1] > 9) {
						portStr[digit-1] = 0;
					}
				} else if(input(INPUT_DOWN)) {
					if(--portStr[digit-1] < 0) {
						portStr[digit-1] = 9;
					}
				} else if(input(INPUT_LEFT)) {
					if(--digit < 1) {
						digit = 5;
					}
				} else if(input(INPUT_RIGHT)) {
					if(++digit > 5) {
						digit = 1;
					}
				}
			}
		}
		break;

		case net::MODE_CLIENT: {
			if(waiting) {
				if(input(INPUT_A)) {
					if(net::connected) {
						sndSelect.play();
						FIGHT->gametype = GAMETYPE_VERSUS;
						setMenu(MENU_SELECT);
					}
				} else if(input(INPUT_B)) {
					if(!net::connected) {
						sndBack.play();
						bgm.play();
						waiting = false;
						net::stop();
					}
				}
			} else if(!digit) {
				if(input(INPUT_A)) {
					if(choice == 0) {
						waiting = true;
						audio::Music::stop();
						sndConStart.play();
						net::start(ip, port);
					} else if(choice == 3) {
						sndSelect.play();

						//Copy IP and port from clipboard
						char _sz_input[80];
						std::string clipboard = os::getClipboard();
						strncpy(_sz_input, clipboard.c_str(), sizeof(_sz_input));

						char* _sz_ip = nullptr;
						char* _sz_port = nullptr;

						//Find the start of the IP and port, if they exist
						//Skip stuff until we reach a digit, that's the IP
						//Find a colon or a null, that's the port
						bool _success = true;
						int _octet_count = 0;
						for(int i = 0; ; i++) {
							if(!_sz_input[i]) {
								if(!_sz_ip) {
									_success = false;
								} else if(!_sz_port) {
									_sz_port = _sz_input + i;
								}
								break;
							}
							if(_sz_input[i] >= '0' && _sz_input[i] <= '9') {
								if(!_sz_ip) {
									_sz_ip = _sz_input + i;
								}
							} else if(_sz_input[i] == ':') {
								_sz_port = _sz_input + i + 1;
								_sz_input[i] = 0;
								break;
							} else if(_sz_input[i] == '.') {
								_octet_count++;
							} else {
								//This isn't an ip!
								_success = false;
								break;
							}
						}

						if(_success && _octet_count == 3) {
							//Update port
							port = atoi(_sz_port);
							if(port == 0) {
								port = DEFAULT_PORT;
							}
							updatePort(false);

							//Initialize IP
							ip = 0;

							//Parse IP (sigh)
							const char* _sz_octet = _sz_ip;
							int _i_octet = 0;
							for(int i = 0; ; i++) {
								if(_sz_ip[i] == '.' || !_sz_ip[i]) {
									_sz_ip[i] = 0;

									//Get the current octet
									int _octet = atoi(_sz_octet);
									if(_octet > 255) {
										_octet = 255;
									}
									ip |= (ubyte_t)_octet << (8 * _i_octet);

									_sz_octet = _sz_ip + i + 1;
									_i_octet++;
								}
								if(_i_octet == 4) {
									break;
								}
							}
							updateIp(false);
						}
					} else {
						sndSelect.play();
						digit = 1;
					}
				} else if(input(INPUT_B)) {
					sndBack.play();
					mode = net::MODE_NONE;
					choice = 1;
				}
				if(input(INPUT_UP)) {
					sndMenu.play();
					if(--choice < 0) {
						choice = 3;
					}
				} else if(input(INPUT_DOWN)) {
					sndMenu.play();
					if(++choice > 3) {
						choice = 0;
					}
				}
			} else {
				if(input(INPUT_A)) {
					//Save changes
					sndSelect.play();
					if(choice == 1) {
						updateIp(true);
					} else if(choice == 2) {
						updatePort(true);
					}
					digit = 0;
				} else if(input(INPUT_B)) {
					//Discard changes
					sndBack.play();
					if(choice == 1) {
						updateIp(false);
					} else if(choice == 2) {
						updatePort(false);
					}
					digit = 0;
				}
				if(input(INPUT_DIRMASK)) {
					sndMenu.play();
				}
				if(choice == 1) {
					if(input(INPUT_UP)) {
						if(++ipStr[digit-1] > 9) {
							ipStr[digit-1] = 0;
						}
					} else if(input(INPUT_DOWN)) {
						if(--ipStr[digit-1] < 0) {
							ipStr[digit-1] = 9;
						}
					} else if(input(INPUT_LEFT)) {
						if(--digit < 1) {
							digit = 12;
						}
					} else if(input(INPUT_RIGHT)) {
						if(++digit > 12) {
							digit = 1;
						}
					}
				} else if(choice == 2) {
					if(input(INPUT_UP)) {
						if(++portStr[digit-1] > 9) {
							portStr[digit-1] = 0;
						}
					} else if(input(INPUT_DOWN)) {
						if(--portStr[digit-1] < 0) {
							portStr[digit-1] = 9;
						}
					} else if(input(INPUT_LEFT)) {
						if(--digit < 1) {
							digit = 5;
						}
					} else if(input(INPUT_RIGHT)) {
						if(++digit > 5) {
							digit = 1;
						}
					}
				}
			}
		}
		break;
		}
	}

	if(!flashTimer && flashDir == 1) {
		sndOn.play();
	}

	flashTimer += flashDir;
	if(flashTimer < 0) {
		flashDir = 0;
		flashTimer = 0;
		setMenu(MENU_TITLE);
	} else if(flashTimer > NET_FLASH_TIME + NET_FLASH_HOLD_TIME + NET_FADE_TIME) {
		flashDir = 0;
		flashTimer = NET_FLASH_TIME + NET_FLASH_HOLD_TIME + NET_FADE_TIME;
	}

	if(flashTimer == NET_FLASH_TIME + NET_FLASH_HOLD_TIME) {
		if(flashDir == 1) {
			bgm.play();
			//graphics::setClearColor(20, 20, 20);
		} else {
			audio::Music::stop();
			//graphics::setClearColor(0, 0, 0);
		}
	}

	barPos--;
	if(barPos < -NET_BAR_SIZE) {
		barPos = WINDOW_HEIGHT + NET_BAR_SIZE;
	}
}

void MenuNetplay::draw() {
	Menu::draw();

	int speed = (flashDir + 1) / 2 + 1;

	if(flashTimer > NET_FLASH_TIME) {
		float xscale = 1.0f;
		if(flashTimer < NET_FLASH_TIME + NET_FLASH_HOLD_TIME + NET_FADE_TIME / speed) {
			float add = (1.0f - (flashTimer - NET_FLASH_TIME - NET_FLASH_HOLD_TIME) / (float)(NET_FADE_TIME/speed));
			add = add * add * add;
			xscale += add;
		}

		graphics::setScale(NET_SCALE * xscale, NET_SCALE);
		imgLogo.draw(WINDOW_WIDTH / 2 - imgLogo.w * NET_SCALE * xscale / 2, WINDOW_HEIGHT / 4 - imgLogo.h * NET_SCALE / 2 + drawShake);

		//Main menu
		switch(mode) {
		case net::MODE_NONE: {
			if(menuFont.exists()) {
				Font::setScale(NET_SCALE * xscale, NET_SCALE);
				menuFont.drawText(WINDOW_WIDTH / 2 - (8 * 8 * NET_SCALE * xscale / 2), WINDOW_HEIGHT / 3 * 2 + drawShake, "Server");
				Font::setScale(NET_SCALE * xscale, NET_SCALE);
				menuFont.drawText(WINDOW_WIDTH / 2 - (8 * 8 * NET_SCALE * xscale / 2), WINDOW_HEIGHT / 3 * 2 + 32 + drawShake, "Client");
			}
			graphics::setScale(xscale, NET_SCALE);
			imgCursor.draw(WINDOW_WIDTH / 2 - (10 * 8 * NET_SCALE * xscale / 2), WINDOW_HEIGHT / 3 * 2 + 32 * choice + drawShake);
		}
		break;

		case net::MODE_SERVER: {
			if(waiting) {
				if(menuFont.exists()) {
					Font::setScale(NET_SCALE);
					if(net::connected) {
						menuFont.drawText(WINDOW_WIDTH / 2 - (23 * 8 * NET_SCALE / 2), WINDOW_HEIGHT / 3 * 2 - 8 * NET_SCALE / 2 + drawShake, "Connection established!");
					} else if(net::running) {
						menuFont.drawText(WINDOW_WIDTH / 2 - (10 * 8 * NET_SCALE / 2), WINDOW_HEIGHT / 3 * 2 - 8 * NET_SCALE / 2 + drawShake, "Waiting...");
					} else {
						menuFont.drawText(WINDOW_WIDTH / 2 - (18 * 8 * NET_SCALE / 2), WINDOW_HEIGHT / 3 * 2 - 8 * NET_SCALE / 2 + drawShake, "An error occurred.");
					}
				}
			} else {
				if(menuFont.exists()) {
					Font::setScale(NET_SCALE);
					menuFont.drawText(WINDOW_WIDTH / 2 - (8 * 8 * NET_SCALE / 2), WINDOW_HEIGHT / 3 * 2 + drawShake, "Start");
					Font::setScale(NET_SCALE);
					menuFont.drawText(WINDOW_WIDTH / 2 - (8 * 8 * NET_SCALE / 2), WINDOW_HEIGHT / 3 * 2 + 32 + drawShake, "Port:");
					for(int i = 0; i < 5; i++) {
						Font::setScale(NET_SCALE);
						menuFont.drawChar(WINDOW_WIDTH / 2 + (8 * 2 * NET_SCALE / 2) + i * 8 * NET_SCALE, WINDOW_HEIGHT / 3 * 2 - ((i+1 == digit) ? 4 : 0) + 32 + drawShake, portStr[i] + '0');
					}
					if(!digit) {
						graphics::setScale(xscale, NET_SCALE);
						imgCursor.draw(WINDOW_WIDTH / 2 - (10 * 8 * NET_SCALE / 2), WINDOW_HEIGHT / 3 * 2 + 32 * choice + drawShake);
					}
				}
			}
		}
		break;

		case net::MODE_CLIENT: {
			if(waiting) {
				if(menuFont.exists()) {
					Font::setScale(NET_SCALE);
					if(net::connected) {
						menuFont.drawText(WINDOW_WIDTH / 2 - (23 * 8 * NET_SCALE / 2), WINDOW_HEIGHT / 3 * 2 - 8 * NET_SCALE / 2 + drawShake, "Connection established!");
					} else if(net::running) {
						menuFont.drawText(WINDOW_WIDTH / 2 - (13 * 8 * NET_SCALE / 2), WINDOW_HEIGHT / 3 * 2 - 8 * NET_SCALE / 2 + drawShake, "Connecting...");
					} else {
						menuFont.drawText(WINDOW_WIDTH / 2 - (18 * 8 * NET_SCALE / 2), WINDOW_HEIGHT / 3 * 2 - 8 * NET_SCALE / 2 + drawShake, "An error occurred.");
					}
				}
			} else {
				if(menuFont.exists()) {
					Font::setScale(NET_SCALE);
					menuFont.drawText(WINDOW_WIDTH / 2 - (8 * 8 * NET_SCALE / 2), WINDOW_HEIGHT / 3 * 2 + drawShake, "Start");
					Font::setScale(NET_SCALE);
					menuFont.drawText(WINDOW_WIDTH / 2 - (8 * 8 * NET_SCALE / 2), WINDOW_HEIGHT / 3 * 2 + 32 + drawShake, "IP:");
					for(int i = 0; i < 4; i++) {
						for(int j = 0; j < 3; j++) {
							Font::setScale(NET_SCALE);
							menuFont.drawChar(WINDOW_WIDTH / 2 + (8 * 2 * NET_SCALE / 2) + (i * 4 + j) * 8 * NET_SCALE, WINDOW_HEIGHT / 3 * 2 - ((choice == 1 && (i * 3 + j)+1 == digit) ? 4 : 0) + 32 + drawShake, ipStr[i * 3 + j] + '0');
						}
						if(i < 3) {
							Font::setScale(NET_SCALE);
							menuFont.drawChar(WINDOW_WIDTH / 2 + (8 * 2 * NET_SCALE / 2) + (i * 4 + 3) * 8 * NET_SCALE, WINDOW_HEIGHT / 3 * 2 + 32 + drawShake, '.');
						}
					}
					Font::setScale(NET_SCALE);
					menuFont.drawText(WINDOW_WIDTH / 2 - (8 * 8 * NET_SCALE / 2), WINDOW_HEIGHT / 3 * 2 + 32 * 2 + drawShake, "Port:");
					for(int i = 0; i < 5; i++) {
						Font::setScale(NET_SCALE);
						menuFont.drawChar(WINDOW_WIDTH / 2 + (8 * 2 * NET_SCALE / 2) + i * 8 * NET_SCALE, WINDOW_HEIGHT / 3 * 2 - ((choice == 2 && i+1 == digit) ? 4 : 0) + 32 * 2 + drawShake, portStr[i] + '0');
					}
					if(!digit) {
						graphics::setScale(xscale, NET_SCALE);
						imgCursor.draw(WINDOW_WIDTH / 2 - (10 * 8 * NET_SCALE / 2), WINDOW_HEIGHT / 3 * 2 + 32 * choice + drawShake);
					}
					Font::setScale(NET_SCALE);
					menuFont.drawText(WINDOW_WIDTH / 2 - (8 * 8 * NET_SCALE / 2), WINDOW_HEIGHT / 3 * 2 + 32 * 3 + drawShake, "Copy from clipboard");
				}
			}
		}
		break;
		}

		glBindTexture(GL_TEXTURE_2D, 0);
		glBegin(GL_QUADS);
		glColor4f(1.0f, 1.0f, 1.0f, 0.0f);
		glVertex3f(WINDOW_WIDTH, barPos, 0);
		glVertex3f(0, barPos, 0);
		glColor4f(1.0f, 1.0f, 1.0f, 0.1f);
		glVertex3f(0, barPos + NET_BAR_SIZE, 0);
		glVertex3f(WINDOW_WIDTH, barPos + NET_BAR_SIZE, 0);
		glEnd();

		graphics::setRender(RENDER_ADDITIVE);
		imgStatic.draw(-util::roll(WINDOW_WIDTH), -util::roll(WINDOW_HEIGHT));
		graphics::setRender(RENDER_MULTIPLY);
		imgScanlines.draw(0, 0);
	}

	if(flashDir) {
		int xoff = (flashTimer * flashTimer * flashTimer) / 5;
		int yoff = 1;
		if(flashTimer > NET_FLASH_TIME / 2) {
			yoff = flashTimer - NET_FLASH_TIME / 2;
			yoff = (yoff * yoff * yoff) / 5;
		}
		float alpha = 1.0f;
		if(flashTimer > NET_FLASH_TIME + NET_FLASH_HOLD_TIME) {
			xoff = WINDOW_WIDTH;
			yoff = WINDOW_WIDTH;
			alpha = 1.0f - (flashTimer - NET_FLASH_TIME - NET_FLASH_HOLD_TIME) / (float)(NET_FADE_TIME/speed);
		}
		//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glBindTexture(GL_TEXTURE_2D, 0);
		glColor4f(1, 1, 1, alpha);
		glBegin(GL_QUADS);
		glVertex3f(WINDOW_WIDTH / 2 - xoff, WINDOW_HEIGHT / 2 - yoff, 0);
		glVertex3f(WINDOW_WIDTH / 2 - xoff, WINDOW_HEIGHT / 2 + yoff, 0);
		glVertex3f(WINDOW_WIDTH / 2 + xoff, WINDOW_HEIGHT / 2 + yoff, 0);
		glVertex3f(WINDOW_WIDTH / 2 + xoff, WINDOW_HEIGHT / 2 - yoff, 0);
		glEnd();
	}
}

void MenuNetplay::reset() {
	Menu::reset();
	choice = 0;
	mode = net::MODE_NONE;
	digit = 0;
	flashDir = 1;
	flashTimer = 0;
	barPos = WINDOW_HEIGHT / 3 * 2;
	waiting = false;
}

void MenuNetplay::updateIp(bool toint) {
	if(toint) {
		ip = 0;
		for(int i = 0; i < 4; i++) {
			int n = 0;
			for(int j = 0; j < 3; j++) {
				n *= 10;
				n += ipStr[i * 3 + j];
			}
			if(n > 255) {
				n = 255;
			}
			ip |= (ubyte_t)n << (8 * i);
		}
		if(!ip) {
			ip = DEFAULT_IP;
		}
		updateIp(false);
	} else {
		int n1 = ip;
		for(int i = 0; i < 4; i++) {
			int n = n1 & 0xFF;
			for(int j = 0; j < 3; j++) {
				ipStr[i * 3 + (2 - j)] = n % 10;
				n /= 10;
			}
			n1 >>= 8;
		}
	}
}

void MenuNetplay::updatePort(bool toint) {
	if(toint) {
		int newport = 0;
		for(int i = 0; i < 5; i++) {
			newport *= 10;
			newport += portStr[i];
		}
		if(newport > 65535) {
			newport = 65535;
		}
		if(newport == 0) {
			newport = DEFAULT_PORT;
		}
		port = newport;
		updatePort(false);
	} else {
		int p = port;
		for(int i = 0; i < 5; i++) {
			portStr[4 - i] = p % 10;
			p /= 10;
		}
	}
}

void MenuNetplay::parseLine(Parser& parser) {
	if(parser.is("BGM", 2)) { //Override superclass
		bgm.createFromFile("", getResource(parser.getArg(1), EXT_MUSIC));
		bgmWait.createFromFile("", getResource(parser.getArg(2), EXT_MUSIC));
	} else if(parser.is("SFX_CONNECT", 2)) {
		sndConStart.createFromFile(getResource(parser.getArg(1), EXT_SOUND));
		sndConSuccess.createFromFile(getResource(parser.getArg(2), EXT_SOUND));
	} else if(parser.is("LOGO", 1)) {
		imgLogo.createFromFile(getResource(parser.getArg(1), EXT_IMAGE));
	} else if(parser.is("SCANLINES", 2)) {
		imgScanlines.createFromFile(getResource(parser.getArg(1), EXT_IMAGE));
		imgStatic.createFromFile(getResource(parser.getArg(2), EXT_IMAGE));
	} else if(parser.is("MENU", 2)) {
		menuFont.createFromFile(getResource(parser.getArg(1), EXT_FONT));
		imgCursor.createFromFile(getResource(parser.getArg(2), EXT_IMAGE));
	} else if(parser.is("SFX_TV", 2)) {
		//Sounds
		sndOn.createFromFile(getResource(parser.getArg(1), EXT_SOUND));
		sndOff.createFromFile(getResource(parser.getArg(2), EXT_SOUND));
	} else {
		Menu::parseLine(parser);
	}
}
#endif

#define CREDITS_OFFSET 20

//CREDITS
MenuCredits::MenuCredits() : Menu("credits") {
	oy = 0;
	timer_start = FPS * 6.35;
	timer_scroll = FPS;
	secret_alpha = 0.0f;
	done = false;

	//Data
	title_r = title_g = title_b = name_r = name_g = name_b = 255;
	c_lines = 0;
	sz_lines = nullptr;
}

MenuCredits::~MenuCredits() {
	util::freeLines(sz_lines);
}

void MenuCredits::think() {
	Menu::think();

	if(timer_start) {
		timer_start--;
	} else if(timer_scroll) {
		timer_scroll--;
	} else if(!done) {
		oy += 0.5;
	} else if(secret_alpha < 1.0f) {
		secret_alpha += 0.05f;
		if(secret_alpha > 1.0f) {
			secret_alpha = 1.0f;
		}
	}

	if(input(INPUT_A)) {
		setMenu(MENU_TITLE);
	}
}

void MenuCredits::draw() {
	Menu::draw();

	if(!timer_start) {
		if(done) {
			const static char* _sz = "Secret character unlocked!";
			font.drawText(WINDOW_WIDTH - font.getTextWidth(_sz) - CREDITS_OFFSET, FLIP(font.img.h * 2), _sz, 255, 255, 255, secret_alpha);
		} else {
			int y = (WINDOW_HEIGHT - logo.h) / 4 - oy;
			if(y - (int)logo.h >= 0) {
				logo.draw(WINDOW_WIDTH - logo.w, y);
			}

			int oy_title = 0;

			for(int i = 0; i < c_lines; i++) {
				if(*sz_lines[i] == ':') {
					oy_title++;
				}

				y = WINDOW_HEIGHT + (font.img.h * (i + oy_title)) - oy;

				if(y + 32 < 0) {
					if(i == c_lines - 1) {
						done = true;
					}
					continue;
				}
				if(y > WINDOW_HEIGHT) {
					break;
				}

				if(*sz_lines[i] == ':') {
					font.drawText(WINDOW_WIDTH - font.getTextWidth(sz_lines[i] + 1) - CREDITS_OFFSET, y, sz_lines[i] + 1, title_r, title_g, title_b);
				} else {
					font.drawText(WINDOW_WIDTH - font.getTextWidth(sz_lines[i]) - CREDITS_OFFSET, y, sz_lines[i], name_r, name_g, name_b);
				}
			}
		}
	}
}

void MenuCredits::reset() {
	Menu::reset();

	done = false;
	oy = 0;
	timer_start = FPS * 6.35;
	timer_scroll = FPS;
	secret_alpha = 0.0f;
}

void MenuCredits::parseLine(Parser& parser) {
	if(parser.is("LOGO", 1)) {
		logo.createFromFile(getResource(parser.getArg(1), EXT_IMAGE));
	} else if(parser.is("CREDITS", 2)) {
		font.createFromFile(getResource(parser.getArg(1), EXT_FONT));
		sz_lines = util::getLinesFromFile(&c_lines, getResource(parser.getArg(2), EXT_TEXT));
	} else if(parser.is("COLOR", 6)) {
		title_r = parser.getArgInt(1);
		title_g = parser.getArgInt(2);
		title_b = parser.getArgInt(3);
		name_r = parser.getArgInt(4);
		name_g = parser.getArgInt(5);
		name_b = parser.getArgInt(6);
	} else {
		Menu::parseLine(parser);
	}
}


//CURSOR DATA
CursorData::CursorData() {
	frameC = 0;
	speed = 0;
	grow = false;
}

CursorData::~CursorData() {
}

//CURSOR
Cursor::Cursor() {
	pos = 0;
	frame = 0;
	timer = 0;

	locked = false;
}

int Cursor::getGroup(int w, int gW, int gH) {
	return (pos % w) / gW + ((pos / w) / gH) * (w / gW);
}

//MENU IMAGE

MenuImage::MenuImage(Image& image_, float x_, float y_, float parallax_, char render_, float xvel_, float yvel_, bool wrap_, int round_) {
	image = std::move(image_);
	x = x_;
	y = y_;
	parallax = parallax_;
	xOrig = x_;
	yOrig = y_;
	render = render_;
	xvel = xvel_;
	yvel = yvel_;
	wrap = wrap_;
	round = round_;

	if(wrap) {
		while(x < 0.0f - image.w) {
			x += image.w;
		}
		while(y < 0.0f - image.h) {
			y += image.h;
		}
		while(x >= 0.0f) {
			x -= image.w;
		}
		while(y >= 0.0f) {
			y -= image.h;
		}
	}

	next = nullptr;
}

MenuImage::~MenuImage() {
	delete next;
}

void MenuImage::think() {
	x += xvel;
	y += yvel;

	//Wrap the wrapping images
	if(wrap) {
		while(x < 0.0f - image.w) {
			x += image.w;
		}
		while(y < 0.0f - image.h) {
			y += image.h;
		}
		while(x >= 0.0f) {
			x -= image.w;
		}
		while(y >= 0.0f) {
			y -= image.h;
		}
	}

	if(next) {
		next->think();
	}
}

void MenuImage::reset() {
	x = xOrig;
	y = yOrig;

	if(next) {
		next->reset();
	}
}

void MenuImage::draw(bool _stage) {
	if(image.exists()) {
		//Draw the image differently if wrapping
		if(wrap) {
			//How many of these are needed to fill the screen?
			int xCount = WINDOW_WIDTH / image.w + 2;
			int yCount = WINDOW_HEIGHT / image.h + 2;

			for(int i = 0; i < xCount; i++) {
				for(int j = 0; j < yCount; j++) {
					graphics::setRender(render);
					image.draw((int)x + i * image.w, (int)y + j * image.h);
				}
			}
		} else {
			graphics::setRender(render);
			if(_stage) {
				if(!round || round - 1 == FIGHT->round) {
					image.draw(x - image.w / 2 + WINDOW_WIDTH / 2 - cameraPos.x * parallax, (WINDOW_HEIGHT - y) - image.h + cameraPos.y * parallax);
				}
			} else {
				image.draw(x, y);
			}
		}
	}
	if(next) {
		next->draw(_stage);
	}
}

