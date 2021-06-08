#include "select.h"

#include "scene.h"
#include "fight.h"
#include "versus.h"

#include "../network.h"
#include "../stage.h"
#include "../sys.h"
#include "../graphics.h"

#include <algorithm>

#include <glad/glad.h>

scene::Select::Select() : Scene("select") {
	width = height = 0;
	gWidth = gHeight = 0;
	gridC = 0;

	cursor_stage = 0;
	cursor_stage_offset = 0;
}

scene::Select::~Select() {}

void scene::Select::init() {
	Scene::init();
}

void scene::Select::think() {
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
	if (cursors[0].lockState != Cursor::CURSOR_LOCKED || cursors[1].lockState != Cursor::CURSOR_LOCKED) {
		for (int cur = 0; cur < 2; cur++) {
			uint16_t input = Fight::madotsuki.frameInput;
			if (cur == 1) {
				input = Fight::poniko.frameInput;
			}
			if (FIGHT->gametype == Fight::GAMETYPE_TRAINING) {
				if (cursors[0].lockState == Cursor::CURSOR_LOCKED) {
					cur = 1;
				}
				else {
					cur = 0;
				}
			}
			int group = cursors[cur].getGroup(width, gWidth, gHeight);

			if (cursors[cur].lockState == Cursor::CURSOR_UNLOCKED) {
				if (input & game::INPUT_DIRMASK) {
					//Old cursor pos SET
					cursors[cur].posOld = cursors[cur].pos;
					cursors[cur].timerPortrait = PORTRAIT_FADE;
				}

				if (input & game::INPUT_LEFT) {
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
				else if (input & game::INPUT_RIGHT) {
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
				else if (input & game::INPUT_UP) {
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
				else if (input & game::INPUT_DOWN) {
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

				if (input & game::INPUT_A) {
					if (gridFighters[cursors[cur].pos] >= 0) {
						if (cur == 0) {
							Fight::madotsuki.palette = 0;
						}
						else {
							Fight::poniko.palette = 0;
						}
						cursors[cur].lockState = Cursor::CURSOR_COLORSWAP;
						newEffect(cur, group);
					}
					else {
						sndInvalid.play();
					}
				}
			}
			else if (cursors[cur].lockState == Cursor::CURSOR_COLORSWAP) {
				if (input & game::INPUT_LEFT) {
					sndMenu.play();

					if (cur == 0) {
						if (Fight::madotsuki.palette == 0) {
							Fight::madotsuki.palette = game::fighters[gridFighters[cursors[0].pos]].nPalettes - 1;
						}
						else {
							Fight::madotsuki.palette--;
						}
					}
					else {
						if (Fight::poniko.palette == 0) {
							Fight::poniko.palette = game::fighters[gridFighters[cursors[1].pos]].nPalettes - 1;
						}
						else {
							Fight::poniko.palette--;
						}
					}
				}
				else if (input & game::INPUT_RIGHT) {
					sndMenu.play();

					if (cur == 0) {
						if (Fight::madotsuki.palette == game::fighters[gridFighters[cursors[0].pos]].nPalettes - 1) {
							Fight::madotsuki.palette = 0;
						}
						else {
							Fight::madotsuki.palette++;
						}
					}
					else {
						if (Fight::poniko.palette == game::fighters[gridFighters[cursors[1].pos]].nPalettes - 1) {
							Fight::poniko.palette = 0;
						}
						else {
							Fight::poniko.palette++;
						}
					}
				}

				if (input & game::INPUT_A) {
					sndSelect.play();
					cursors[cur].lockState = Cursor::CURSOR_LOCKED;
				}
			}

			if (input & game::INPUT_B) {
				if (FIGHT->gametype == scene::Fight::GAMETYPE_TRAINING) {
					if (cursors[0].lockState == Cursor::CURSOR_UNLOCKED) {
						sndBack.play();
						setScene(SCENE_TITLE);
					}
					else if (cursors[cur].lockState == Cursor::CURSOR_UNLOCKED) {
						sndBack.play();
						cur--;
						cursors[cur].lockState = Cursor::CURSOR_COLORSWAP;
					}
					else if (cursors[cur].lockState == Cursor::CURSOR_COLORSWAP) {
						cursors[cur].lockState = Cursor::CURSOR_UNLOCKED;

						if (!curData.empty()) {
							group = cursors[cur].getGroup(width, gWidth, gHeight);
							curData[group].sndDeselect.play();
							curData[group].sndSelect.stop();
						}
					}
				}
				else {
					if (cursors[cur].lockState == Cursor::CURSOR_LOCKED) {
						sndBack.play();
						cursors[cur].lockState = Cursor::CURSOR_COLORSWAP;
					}
					else if (cursors[cur].lockState == Cursor::CURSOR_COLORSWAP) {
						cursors[cur].lockState = Cursor::CURSOR_UNLOCKED;
						if (!curData.empty()) {
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

			if (FIGHT->gametype == scene::Fight::GAMETYPE_TRAINING) {
				break;
			}
		}
	}
	else {
		if (input(game::INPUT_LEFT)) {
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

		if (input(game::INPUT_RIGHT)) {
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


		if (input(game::INPUT_UP)) {
			sndMenu.play();
			if (cursor_stage < 10) {
				cursor_stage += 10;
			}
			else {
				cursor_stage -= 10;
			}
		}

		if (input(game::INPUT_DOWN)) {
			sndMenu.play();
			if (cursor_stage >= 11) {
				cursor_stage -= 10;
			}
			else {
				cursor_stage += 10;
			}
		}

		if (FIGHT->gametype == Fight::GAMETYPE_TRAINING) {
			if (input(game::INPUT_B)) {
				cursors[1].lockState = Cursor::CURSOR_COLORSWAP;

				if (!curData.empty()) {
					int group = cursors[1].getGroup(width, gWidth, gHeight);
					curData[group].sndDeselect.play();
					curData[group].sndSelect.stop();
				}
			}
		}
		else {
			if (input(game::INPUT_B)) {
				sndBack.play();
				cursors[0].lockState = Cursor::CURSOR_COLORSWAP;
				cursors[1].lockState = Cursor::CURSOR_COLORSWAP;

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

		if (input(game::INPUT_A)) {
			//Start game!
			Fight::madotsuki.fighter = &game::fighters[gridFighters[cursors[0].pos]];
			Fight::poniko.fighter = &game::fighters[gridFighters[cursors[1].pos]];

			(reinterpret_cast<Versus*>(scenes[SCENE_VERSUS].get()))->portraits[0] = &Fight::madotsuki.fighter->portrait;

			(reinterpret_cast<Versus*>(scenes[SCENE_VERSUS].get()))->portraits[1] = &Fight::poniko.fighter->portrait;

			Stage::stage = cursor_stage;
			setScene(SCENE_VERSUS);
			sndSelect.play();
		}
	}
}

void scene::Select::reset() {
	Scene::reset();

	for (int i = 0; i < 2; i++) {
		cursors[i].lockState = Cursor::CURSOR_UNLOCKED;
		cursors[i].timer = 0;
		cursors[i].timerPortrait = 0;
		cursors[i].frame = 0;
	}
	cursors[0].pos = cursors[0].posDefault;
	cursors[1].pos = cursors[1].posDefault;

	cursor_stage = 0;
}

void scene::Select::draw() const {
	Scene::draw();

	//Draw portraits first
	if (cursors[0].lockState == Cursor::CURSOR_LOCKED || FIGHT->gametype == Fight::GAMETYPE_VERSUS) {
		if (cursors[1].timerPortrait) {
			if (gridFighters[cursors[1].posOld] >= 0) {
				graphics::setColor(255, 255, 255, (float)(cursors[1].timerPortrait) / PORTRAIT_FADE);
				game::fighters[gridFighters[cursors[1].posOld]].portrait.draw(sys::WINDOW_WIDTH - game::fighters[gridFighters[cursors[1].posOld]].portrait.w + (PORTRAIT_FADE - cursors[1].timerPortrait), 0, true);
			}
		}
		if (gridFighters[cursors[1].pos] >= 0) {
			graphics::setColor(255, 255, 255, (float)(PORTRAIT_FADE - cursors[1].timerPortrait) / PORTRAIT_FADE);
			game::fighters[gridFighters[cursors[1].pos]].portrait.draw(sys::WINDOW_WIDTH - game::fighters[gridFighters[cursors[1].pos]].portrait.w + cursors[1].timerPortrait, 0, true);
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
	std::for_each(gui.cbegin(), gui.cend(),
		[](const SceneImage& si) {si.draw(false); }
	);

	if (cursors[1].lockState >= Cursor::CURSOR_COLORSWAP) {
		if (gridFighters[cursors[1].pos] >= 0) {
			game::Fighter& fighter = game::fighters[gridFighters[cursors[1].pos]];
			sprite::Sprite& spr = fighter.sprites[0];
			AtlasSprite sprAtlas = fighter.sprites[0].atlas->getSprite(spr.atlas_sprite);

			graphics::setPalette(fighter.palettes[Fight::poniko.palette], 1.0f, 1.0f, 1.0f, 1.0f, 0.0f);
			fighter.sprites[0].atlas->draw(spr.atlas_sprite, sys::WINDOW_WIDTH - 50 + spr.x - sprAtlas.w, sys::WINDOW_HEIGHT - 40 - spr.y - sprAtlas.h, true);
			glUseProgram(0);
		}
	}
	if (cursors[0].lockState >= Cursor::CURSOR_COLORSWAP) {
		if (gridFighters[cursors[0].pos] >= 0) {
			game::Fighter& fighter = game::fighters[gridFighters[cursors[0].pos]];
			sprite::Sprite& spr = fighter.sprites[0];
			AtlasSprite sprAtlas = fighter.sprites[0].atlas->getSprite(spr.atlas_sprite);

			graphics::setPalette(fighter.palettes[Fight::madotsuki.palette], 1.0f, 1.0f, 1.0f, 1.0f, 0.0f);
			fighter.sprites[0].atlas->draw(spr.atlas_sprite, 50 - spr.x, sys::WINDOW_HEIGHT - 40 - spr.y - sprAtlas.h, false);
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
	if (!curData.empty()) {
		int count = 1;
		if (FIGHT->gametype == Fight::GAMETYPE_TRAINING && cursors[0].lockState != Cursor::CURSOR_LOCKED) {
			count = 0;
		}
		for (int i = 0; i <= count; i++) {
			int group = cursors[i].getGroup(width, gWidth, gHeight);;

			if (cursors[i].lockState == Cursor::CURSOR_UNLOCKED) {
				graphics::setColor(cursors[i].r, cursors[i].g, cursors[i].b);
			}
			curData[group].img.draw(grid[cursors[i].pos].x + curData[group].off.x, grid[cursors[i].pos].y + curData[group].off.y);

			//Draw the effects
			drawEffect(i, group, grid[cursors[i].pos].x, grid[cursors[i].pos].y);
		}
	}

	//Finally draw the stage selection screen
	if (cursors[0].lockState == Cursor::CURSOR_LOCKED && cursors[1].lockState == Cursor::CURSOR_LOCKED) {
		//Darken background
		glBindTexture(GL_TEXTURE_2D, 0);
		glColor4f(0.0f, 0.0f, 0.0f, 0.7f);
		glBegin(GL_QUADS);
		glVertex3f(0.0f, 0.0f, 0.0f);
		glVertex3f(0.0f, sys::WINDOW_HEIGHT, 0.0f);
		glVertex3f(sys::WINDOW_WIDTH, sys::WINDOW_HEIGHT, 0.0f);
		glVertex3f(sys::WINDOW_WIDTH, 0.0f, 0.0f);
		glEnd();

		//Draw the stage list
		for (int i = 0; i < 20; i++) {
			int x = static_cast<int>(38 + (8 + 76) * (i % 10 + 3 - cursor_stage % 10) + cursor_stage_offset);
			int y = 150 + (8 + 50) * (i / 10);
			cursor_stage_offset *= 0.95f;
			if (!Stage::stages[i].thumbnail.isPlaying())
				Stage::stages[i].thumbnail.setPlaying(true);
			if (cursor_stage == i) {
				graphics::setColor(255, 255, 255, 1.0f);
				Stage::stages[i].thumbnail.draw(x, y);
			}
			else {
				graphics::setColor(127, 127, 127, 1.0f);
				Stage::stages[i].thumbnail.draw(x, y);
			}
		}
	}
}

void scene::Select::newEffect(int player, int group) {
	if (!curData.empty()) {
		curData[group].sndSelect.play();
		curData[group].sndDeselect.stop();
	}

	cursors[player].frame = 1;
}

void scene::Select::drawEffect(int player, int group, int _x, int _y, bool spr) const {
	if (cursors[player].frame) {
		float scale = 1.0f;
		float alpha = 1.0f - (cursors[player].frame - 1) / (float)curData[group].frameC;;
		if (curData[group].grow) {
			scale += (cursors[player].frame - 1) * 0.1f;
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

void scene::Select::parseLine(Parser& parser) {
	int argc = parser.getArgC();
	if (parser.is("GRID", 4)) {
		//Make the grids!
		width = parser.getArgInt(1);
		height = parser.getArgInt(2);

		grid.resize(width * height);
		gridFighters.resize(width * height);

		//Now for the groups...
		gWidth = parser.getArgInt(3);
		gHeight = parser.getArgInt(4);

		int gSize = (width / gWidth) * (height / gHeight);
		curData.resize(gSize);

		//Set the p2 cursor to the width - 1
		cursors[1].pos = width - 1;
		cursors[1].posOld = width - 1;
	}
	else if (parser.is("CURSOR", 10)) {
		//Load up the cursor

		//Get the group num
		int group = parser.getArgInt(1) - 1;

		//Cursor Image
		curData[group].img.createFromFile(getResource(parser.getArg(2), Parser::EXT_IMAGE));

		//X & Y offsets
		curData[group].off.x = parser.getArgInt(3);
		curData[group].off.y = parser.getArgInt(4);

		//Effect stuff
		curData[group].imgSelect.createFromFile(getResource(parser.getArg(5), Parser::EXT_IMAGE));
		curData[group].frameC = parser.getArgInt(6);
		curData[group].speed = parser.getArgInt(7);
		curData[group].grow = parser.getArgBool(8, false);

		//Sounds
		curData[group].sndSelect.createFromFile(getResource(parser.getArg(9), Parser::EXT_SOUND));
		curData[group].sndDeselect.createFromFile(getResource(parser.getArg(10), Parser::EXT_SOUND));
	}
	else if (parser.is("CHAR", 1)) {
		//Add to the grids
		gridFighters[gridC] = -1;

		//Is it a null?
		if (!parser.getArg(1).compare("null")) {
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
		for (int i = 0, fightersSize = game::fighters.size(); i < fightersSize; i++) {
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
		Image::Render render = Image::Render::NORMAL;
		float xvel = 0.0f;
		float yvel = 0.0f;
		bool wrap = false;
		if (argc > 4) {
			std::string szRender = parser.getArg(4);
			if (!szRender.compare("additive")) {
				render = Image::Render::ADDITIVE;
			}
			else if (!szRender.compare("subtractive")) {
				render = Image::Render::SUBTRACTIVE;
			}
			else if (!szRender.compare("multiply")) {
				render = Image::Render::MULTIPLY;
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
		imgData.createFromFile(getResource(parser.getArg(1), Parser::EXT_IMAGE));
		if (!imgData.exists()) {
			return;
		}
		gui.emplace_back(imgData, x, y, 1.0f, render, xvel, yvel, wrap, 0);
	}
	else if (parser.is("STAGES", 1)) {
		//Load the font
		font_stage.createFromFile(getResource(parser.getArg(1), Parser::EXT_FONT));
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
scene::CursorData::CursorData() {
	frameC = 0;
	speed = 0;
	grow = false;
}

scene::CursorData::~CursorData() {
}

scene::CursorData::CursorData(CursorData&& other) noexcept :
	off(std::move(other.off)),
	img(std::move(other.img)), imgSelect(std::move(other.imgSelect)),
	sndSelect(std::move(other.sndSelect)), sndDeselect(std::move(other.sndDeselect)),
	frameC(other.frameC), speed(other.speed), grow(other.grow)
{}

scene::CursorData& scene::CursorData::operator=(CursorData& other) noexcept {
	off = std::move(other.off);
	img = std::move(other.img);
	imgSelect = std::move(other.imgSelect);
	sndSelect = std::move(other.sndSelect);
	sndDeselect = std::move(other.sndDeselect);

	frameC = other.frameC;
	speed = other.speed;
	grow = other.grow;

	return *this;
}

//CURSOR
scene::Cursor::Cursor() {
	pos = posOld = posDefault = 0;
	frame = 0;
	timer = 0;

	timerPortrait = 0;

	lockState = CURSOR_UNLOCKED;

	r = g = b = 0;
}

int scene::Cursor::getGroup(int w, int gW, int gH) const {
	return (pos % w) / gW + ((pos / w) / gH) * (w / gW);
}
