#include "select.h"

#include "scene.h"

#include "../network.h"
#include "../stage.h"

//CHARACTER SELECT
#define PORTRAIT_FADE 50

SceneSelect::SceneSelect() : Scene("select") {
	width = height = 0;
	gWidth = gHeight = 0;
	gui = nullptr;
	grid = nullptr;
	gridFighters = nullptr;
	gridC = 0;

	curData = nullptr;

	cursor_stage = 0;
	cursor_stage_offset = 0;
}

SceneSelect::~SceneSelect() {
	delete gui;
	delete[] curData;
	delete[] grid;
	delete[] gridFighters;
}

void SceneSelect::init() {
	Scene::init();
}

void SceneSelect::think() {
	Scene::think();

	//Visual effects animations

	for (int i = 0; i < 2; i++) {
		if (cursors[i].timerPortrait) {
			if (cursors[i].timerPortrait == 1) {
				cursors[i].timerPortrait = 0;
			}
			else {
				cursors[i].timerPortrait *= 0.8;
			}
		}
	}

	//Move cursor
	if (cursors[0].lockState != CURSOR_LOCKED || cursors[1].lockState != CURSOR_LOCKED) {
		for (int cur = 0; cur < 2; cur++) {
			uint16_t input = madotsuki.frameInput;
			if (cur == 1) {
				input = poniko.frameInput;
			}
			if (FIGHT->gametype == GAMETYPE_TRAINING) {
				if (cursors[0].lockState == CURSOR_LOCKED) {
					cur = 1;
				}
				else {
					cur = 0;
				}
			}
			int group = cursors[cur].getGroup(width, gWidth, gHeight);

			if (cursors[cur].lockState == CURSOR_UNLOCKED) {
				if (input & INPUT_DIRMASK) {
					//Old cursor pos SET
					cursors[cur].posOld = cursors[cur].pos;
					cursors[cur].timerPortrait = PORTRAIT_FADE;
				}

				if (input & INPUT_LEFT) {
					sndMenu.play();

					do {
						if (cursors[cur].pos % width == 0) {
							cursors[cur].pos += width - 1;
						}
						else {
							cursors[cur].pos--;
						}
					} while (grid[cursors[cur].pos].x == -1);

					//Check cell limits
					if (cursors[cur].pos >= gridC) {
						cursors[cur].pos += gridC % width - (cursors[cur].pos % width) - 1;
					}
				}
				else if (input & INPUT_RIGHT) {
					sndMenu.play();

					do {
						if (cursors[cur].pos % width == width - 1) {
							cursors[cur].pos -= width - 1;
						}
						else {
							cursors[cur].pos++;
						}
					} while (grid[cursors[cur].pos].x == -1);

					//Check cell limits
					if (cursors[cur].pos >= gridC) {
						cursors[cur].pos -= cursors[cur].pos % width;
					}
				}
				else if (input & INPUT_UP) {
					sndMenu.play();

					do {
						if (cursors[cur].pos / width == 0) {
							cursors[cur].pos += width * (height - 1);
						}
						else {
							cursors[cur].pos -= width;
						}
					} while (grid[cursors[cur].pos].x == -1);

					//Check cell limits
					if (cursors[cur].pos >= gridC) {
						if (cursors[cur].pos % width < gridC % width) {
							cursors[cur].pos -= width * (gridC / width - 1);
						}
						else {
							cursors[cur].pos -= width * (gridC / width);
						}
					}
				}
				else if (input & INPUT_DOWN) {
					sndMenu.play();

					do {
						if (cursors[cur].pos / width == height - 1) {
							cursors[cur].pos -= width * (height - 1);
						}
						else {
							cursors[cur].pos += width;
						}
					} while (grid[cursors[cur].pos].x == -1);

					//Check cell limits
					if (cursors[cur].pos >= gridC) {
						cursors[cur].pos = cursors[cur].pos % width;
					}
				}

				if (input & INPUT_A) {
					if (gridFighters[cursors[cur].pos] >= 0) {
						if (cur == 0) {
							madotsuki.palette = 0;
						}
						else {
							poniko.palette = 0;
						}
						cursors[cur].lockState = CURSOR_COLORSWAP;
						newEffect(cur, group);
					}
					else {
						sndInvalid.play();
					}
				}
			}
			else if (cursors[cur].lockState == CURSOR_COLORSWAP) {
				if (input & INPUT_LEFT) {
					sndMenu.play();

					if (cur == 0) {
						if (madotsuki.palette == 0) {
							madotsuki.palette = game::fighters[gridFighters[cursors[0].pos]].nPalettes - 1;
						}
						else {
							madotsuki.palette--;
						}
					}
					else {
						if (poniko.palette == 0) {
							poniko.palette = game::fighters[gridFighters[cursors[1].pos]].nPalettes - 1;
						}
						else {
							poniko.palette--;
						}
					}
				}
				else if (input & INPUT_RIGHT) {
					sndMenu.play();

					if (cur == 0) {
						if (madotsuki.palette == game::fighters[gridFighters[cursors[0].pos]].nPalettes - 1) {
							madotsuki.palette = 0;
						}
						else {
							madotsuki.palette++;
						}
					}
					else {
						if (poniko.palette == game::fighters[gridFighters[cursors[1].pos]].nPalettes - 1) {
							poniko.palette = 0;
						}
						else {
							poniko.palette++;
						}
					}
				}

				if (input & INPUT_A) {
					sndSelect.play();
					cursors[cur].lockState = CURSOR_LOCKED;
				}
			}

			if (input & INPUT_B) {
				if (FIGHT->gametype == GAMETYPE_TRAINING) {
					if (cursors[0].lockState == CURSOR_UNLOCKED) {
						sndBack.play();
						setScene(SCENE_TITLE);
					}
					else if (cursors[cur].lockState == CURSOR_UNLOCKED) {
						sndBack.play();
						cur--;
						cursors[cur].lockState = CURSOR_COLORSWAP;
					}
					else if (cursors[cur].lockState == CURSOR_COLORSWAP) {
						cursors[cur].lockState = CURSOR_UNLOCKED;

						if (curData) {
							group = cursors[cur].getGroup(width, gWidth, gHeight);
							curData[group].sndDeselect.play();
							curData[group].sndSelect.stop();
						}
					}
				}
				else {
					if (cursors[cur].lockState == CURSOR_LOCKED) {
						sndBack.play();
						cursors[cur].lockState = CURSOR_COLORSWAP;
					}
					else if (cursors[cur].lockState == CURSOR_COLORSWAP) {
						cursors[cur].lockState = CURSOR_UNLOCKED;
						if (curData) {
							group = cursors[cur].getGroup(width, gWidth, gHeight);
							curData[group].sndDeselect.play();
							curData[group].sndSelect.stop();
						}
					}
					else {
						if (!net::connected) {
							sndBack.play();
							setScene(SCENE_TITLE);
						}
					}
				}
			}

			if (FIGHT->gametype == GAMETYPE_TRAINING) {
				break;
			}
		}
	}
	else {
		if (input(INPUT_LEFT)) {
			sndMenu.play();
			if (cursor_stage % 10 == 0) {
				cursor_stage += 9;
				cursor_stage_offset += 76 * 10;
			}
			else {
				cursor_stage--;
				cursor_stage_offset += -76;
			}
		}

		if (input(INPUT_RIGHT)) {
			sndMenu.play();
			if (cursor_stage % 10 == 9) {
				cursor_stage -= 9;
				cursor_stage_offset += -76 * 10;
			}
			else {
				cursor_stage++;
				cursor_stage_offset += 76;
			}
		}


		if (input(INPUT_UP)) {
			sndMenu.play();
			if (cursor_stage < 10) {
				cursor_stage += 10;
			}
			else {
				cursor_stage -= 10;
			}
		}

		if (input(INPUT_DOWN)) {
			sndMenu.play();
			if (cursor_stage >= 11) {
				cursor_stage -= 10;
			}
			else {
				cursor_stage += 10;
			}
		}

		if (FIGHT->gametype == GAMETYPE_TRAINING) {
			if (input(INPUT_B)) {
				cursors[1].lockState = CURSOR_COLORSWAP;

				if (curData) {
					int group = cursors[1].getGroup(width, gWidth, gHeight);
					curData[group].sndDeselect.play();
					curData[group].sndSelect.stop();
				}
			}
		}
		else {
			if (input(INPUT_B)) {
				sndBack.play();
				cursors[0].lockState = CURSOR_COLORSWAP;
				cursors[1].lockState = CURSOR_COLORSWAP;

				/*if(curData) {
					int group = cursors[0].getGroup(width, gWidth, gHeight);
					curData[group].sndDeselect.play();
					curData[group].sndSelect.stop();
					group = cursors[1].getGroup(width, gWidth, gHeight);
					curData[group].sndDeselect.play();
					curData[group].sndSelect.stop();
				}*/
			}
		}

		if (input(INPUT_A)) {
			//Start game!
			madotsuki.fighter = &game::fighters[gridFighters[cursors[0].pos]];
			poniko.fighter = &game::fighters[gridFighters[cursors[1].pos]];

			((SceneVersus*)scenes[SCENE_VERSUS])->portraits[0] = &madotsuki.fighter->portrait;

			((SceneVersus*)scenes[SCENE_VERSUS])->portraits[1] = &poniko.fighter->portrait;

			stage = cursor_stage;
			setScene(SCENE_VERSUS);
			sndSelect.play();
		}
	}
}

void SceneSelect::reset() {
	Scene::reset();

	for (int i = 0; i < 2; i++) {
		cursors[i].lockState = CURSOR_UNLOCKED;
		cursors[i].timer = 0;
		cursors[i].timerPortrait = 0;
		cursors[i].frame = 0;
	}
	cursors[0].pos = cursors[0].posDefault;
	cursors[1].pos = cursors[1].posDefault;

	cursor_stage = 0;
}

void SceneSelect::draw() {
	Scene::draw();

	//Draw portraits first
	if (cursors[0].lockState == CURSOR_LOCKED || FIGHT->gametype == GAMETYPE_VERSUS) {
		if (cursors[1].timerPortrait) {
			if (gridFighters[cursors[1].posOld] >= 0) {
				graphics::setColor(255, 255, 255, (float)(cursors[1].timerPortrait) / PORTRAIT_FADE);
				game::fighters[gridFighters[cursors[1].posOld]].portrait.draw(WINDOW_WIDTH - game::fighters[gridFighters[cursors[1].posOld]].portrait.w + (PORTRAIT_FADE - cursors[1].timerPortrait), 0, true);
			}
		}
		if (gridFighters[cursors[1].pos] >= 0) {
			graphics::setColor(255, 255, 255, (float)(PORTRAIT_FADE - cursors[1].timerPortrait) / PORTRAIT_FADE);
			game::fighters[gridFighters[cursors[1].pos]].portrait.draw(WINDOW_WIDTH - game::fighters[gridFighters[cursors[1].pos]].portrait.w + cursors[1].timerPortrait, 0, true);
		}
	}
	if (cursors[0].timerPortrait) {
		if (gridFighters[cursors[0].posOld] >= 0) {
			graphics::setColor(255, 255, 255, (float)(cursors[0].timerPortrait) / PORTRAIT_FADE);
			game::fighters[gridFighters[cursors[0].posOld]].portrait.draw(0 - (PORTRAIT_FADE - cursors[0].timerPortrait), 0);
		}
	}
	if (gridFighters[cursors[0].pos] >= 0) {
		graphics::setColor(255, 255, 255, (float)(PORTRAIT_FADE - cursors[0].timerPortrait) / PORTRAIT_FADE);
		game::fighters[gridFighters[cursors[0].pos]].portrait.draw(0 - cursors[0].timerPortrait, 0);
	}

	//Now the GUI
	if (gui) {
		gui->draw(false);
	}

	if (cursors[1].lockState >= CURSOR_COLORSWAP) {
		if (gridFighters[cursors[1].pos] >= 0) {
			game::Fighter& fighter = game::fighters[gridFighters[cursors[1].pos]];
			sprite::Sprite& spr = fighter.sprites[0];
			AtlasSprite sprAtlas = fighter.sprites[0].atlas->getSprite(spr.atlas_sprite);

			graphics::setPalette(fighter.palettes[poniko.palette], 1.0f, 1.0f, 1.0f, 1.0f, 0.0f);
			fighter.sprites[0].atlas->draw(spr.atlas_sprite, WINDOW_WIDTH - 50 + spr.x - sprAtlas.w, WINDOW_HEIGHT - 40 - spr.y - sprAtlas.h, true);
			glUseProgram(0);
		}
	}
	if (cursors[0].lockState >= CURSOR_COLORSWAP) {
		if (gridFighters[cursors[0].pos] >= 0) {
			game::Fighter& fighter = game::fighters[gridFighters[cursors[0].pos]];
			sprite::Sprite& spr = fighter.sprites[0];
			AtlasSprite sprAtlas = fighter.sprites[0].atlas->getSprite(spr.atlas_sprite);

			graphics::setPalette(fighter.palettes[madotsuki.palette], 1.0f, 1.0f, 1.0f, 1.0f, 0.0f);
			fighter.sprites[0].atlas->draw(spr.atlas_sprite, 50 - spr.x, WINDOW_HEIGHT - 40 - spr.y - sprAtlas.h, false);
			glUseProgram(0);
		}
	}

	//Draw the select sprites
	for (int i = 0; i < gridC; i++) {
		if (gridFighters[i] >= 0) {
			game::fighters[gridFighters[i]].select.draw(grid[i].x, grid[i].y);
		}
	}

	//Cursor
	//Get the current group
	if (curData) {
		int count = 1;
		if (FIGHT->gametype == GAMETYPE_TRAINING && cursors[0].lockState != CURSOR_LOCKED) {
			count = 0;
		}
		for (int i = 0; i <= count; i++) {
			int group = cursors[i].getGroup(width, gWidth, gHeight);;

			if (cursors[i].lockState == CURSOR_UNLOCKED) {
				graphics::setColor(cursors[i].r, cursors[i].g, cursors[i].b);
			}
			curData[group].img.draw(grid[cursors[i].pos].x + curData[group].off.x, grid[cursors[i].pos].y + curData[group].off.y);

			//Draw the effects
			drawEffect(i, group, grid[cursors[i].pos].x, grid[cursors[i].pos].y);
		}
	}

	//Finally draw the stage selection screen
	if (cursors[0].lockState == CURSOR_LOCKED && cursors[1].lockState == CURSOR_LOCKED) {
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
		for (int i = 0; i < 20; i++) {
			int x = 38 + (8 + 76) * (i % 10 + 3 - cursor_stage % 10) + cursor_stage_offset;
			int y = 150 + (8 + 50) * (i / 10);
			cursor_stage_offset *= 0.95;
			if (!stages[i].thumbnail.isPlaying())
				stages[i].thumbnail.setPlaying(true);
			if (cursor_stage == i) {
				graphics::setColor(255, 255, 255, 1.0f);
				stages[i].thumbnail.draw(x, y);
			}
			else {
				graphics::setColor(127, 127, 127, 1.0f);
				stages[i].thumbnail.draw(x, y);
			}
		}
	}
}

void SceneSelect::newEffect(int player, int group) {
	if (curData) {
		curData[group].sndSelect.play();
		curData[group].sndDeselect.stop();
	}

	cursors[player].frame = 1;
}

void SceneSelect::drawEffect(int player, int group, int _x, int _y, bool spr) {
	if (cursors[player].frame) {
		float scale = 1.0f;
		float alpha = 1.0f - (cursors[player].frame - 1) / (float)curData[group].frameC;;
		if (curData[group].grow) {
			scale += (cursors[player].frame - 1) * 0.1;
		}
		int x = (cursors[player].frame - 1) % (curData[group].imgSelect.h / 96);

		graphics::setRect(0, x * 96, 96, 96);
		graphics::setColor(255, 255, 255, alpha);

		if (spr) {
			graphics::setScale(scale * 2);
			curData[group].imgSelect.drawSprite(_x - (96 * scale), _y - (96 * scale));
		}
		else {
			graphics::setScale(scale);
			curData[group].imgSelect.draw(_x - (96 / 2 * scale) + 26 / 2, _y - (96 / 2 * scale) + 29 / 2);
		}

		if (++cursors[player].timer > curData[group].speed) {
			cursors[player].timer = 0;
			if (++cursors[player].frame > curData[group].frameC) {
				cursors[player].frame = 0;
			}
		}
	}
}

void SceneSelect::parseLine(Parser& parser) {
	int argc = parser.getArgC();
	if (parser.is("GRID", 4)) {
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
	}
	else if (parser.is("CURSOR", 10)) {
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
	}
	else if (parser.is("CHAR", 1)) {
		//Add to the grids
		gridFighters[gridC] = -1;

		//Is it a null?
		if (!strcmp(parser.getArg(1), "null")) {
			grid[gridC].x = -1;
			grid[gridC].y = -1;
			gridC++;
			return;
		}

		if (argc < 3) {
			return;
		}
		if (gridC >= width * height) {
			return;
		}

		//Get fighter
		for (int i = 0; i < FIGHTERS_MAX; i++) {
			if (!game::fighters[i].name.compare(parser.getArg(1))) {
				gridFighters[gridC] = i;
				break;
			}
		}

		//Position
		grid[gridC].x = parser.getArgInt(2);
		grid[gridC].y = parser.getArgInt(3);

		gridC++;
	}
	else if (parser.is("SELECT", 2)) {
		float x = parser.getArgFloat(2);
		float y = parser.getArgFloat(3);
		char render = RENDER_NORMAL;
		float xvel = 0.0f;
		float yvel = 0.0f;
		bool wrap = false;
		if (argc > 4) {
			const char* szRender = parser.getArg(4);
			if (!strcmp(szRender, "additive")) {
				render = RENDER_ADDITIVE;
			}
			else if (!strcmp(szRender, "subtractive")) {
				render = RENDER_SUBTRACTIVE;
			}
			else if (!strcmp(szRender, "multiply")) {
				render = RENDER_MULTIPLY;
			}
			if (argc > 5) {
				xvel = parser.getArgFloat(5);
				if (argc > 6) {
					yvel = parser.getArgFloat(6);
					if (argc > 7) {
						wrap = parser.getArgBool(7, false);
					}
				}
			}
		}

		//Add a new image
		Image imgData;
		imgData.createFromFile(getResource(parser.getArg(1), EXT_IMAGE));
		if (!imgData.exists()) {
			return;
		}
		gui = new SceneImage(imgData, x, y, 1.0f, render, xvel, yvel, wrap, 0);
	}
	else if (parser.is("STAGES", 1)) {
		//Load the font
		font_stage.createFromFile(getResource(parser.getArg(1), EXT_FONT));
	}
	else if (parser.is("PLAYER", 6)) {
		//Load the player
		int p = parser.getArgInt(1) - 1;

		//Set the default position of the cursor
		cursors[p].posDefault = parser.getArgInt(3) * width + parser.getArgInt(2);
		cursors[p].pos = cursors[p].posDefault;

		//Colors
		cursors[p].r = parser.getArgInt(4);
		cursors[p].g = parser.getArgInt(5);
		cursors[p].b = parser.getArgInt(6);
	}
	else {
		Scene::parseLine(parser);
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

	lockState = CURSOR_UNLOCKED;
}

int Cursor::getGroup(int w, int gW, int gH) {
	return (pos % w) / gW + ((pos / w) / gH) * (w / gW);
}
