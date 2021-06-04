#include "fight.h"

#include "scene.h"

#include "../stage.h"
#include "../effect.h"
#include "../sys.h"

#include <glad/glad.h>

game::Player SceneFight::madotsuki;
game::Player SceneFight::poniko;
util::Vector SceneFight::cameraPos(0, 0);
util::Vector SceneFight::idealCameraPos(0, 0);
util::Vector SceneFight::cameraShake(0, 0);

int SceneFight::framePauseTimer = 0;
int SceneFight::frameShakeTimer = 0;

void SceneFight::pause(int frames) {
	framePauseTimer += frames;
}

void SceneFight::shake(int frames) {
	frameShakeTimer += frames;
}

SceneMeter::SceneMeter() {
}

SceneMeter::~SceneMeter() {
}

void SceneMeter::draw(float pct, bool mirror, bool flip) const {
	if (pct > 0) {
		if (flip) {
			graphics::setRect(0, 0, img.w * pct, img.h);
		}
		else {
			graphics::setRect(img.w * (1 - pct), 0, img.w * pct + 1, img.h);
		}

		if (mirror) {
			if (flip) {
				img.draw(sys::WINDOW_WIDTH - img.w - pos.x + img.w * (1 - pct), pos.y, true);
			}
			else {
				//Hack
				if (pct == 1.0f) {
					img.draw(sys::WINDOW_WIDTH - img.w - pos.x, pos.y, true);
				}
				else {
					img.draw(sys::WINDOW_WIDTH - img.w - pos.x + 1, pos.y, true);
				}
			}
		}
		else {
			if (flip) {
				img.draw(pos.x, pos.y);
			}
			else {
				img.draw(pos.x + img.w * (1 - pct), pos.y);
			}
		}
	}
}

SceneFight::SceneFight() : Scene("fight") {
	gametype = GAMETYPE_TRAINING;
	reset();
}

SceneFight::~SceneFight() {
}

void SceneFight::init() {
	Scene::init();
}

void SceneFight::parseLine(Parser& parser) {
	if (parser.is("HUD", 1)) {
		hud.createFromFile(getResource(parser.getArg(1), Parser::EXT_IMAGE));
	}
	else if (parser.is("HUD_TAG", 1)) {
		hud_tag.createFromFile(getResource(parser.getArg(1), Parser::EXT_IMAGE));
	}
	else if (parser.is("PORTRAITS", 1)) {
		portraits.createFromFile(getResource(parser.getArg(1), Parser::EXT_IMAGE));
	}
	else if (parser.is("PORTRAITS_TAG", 1)) {
		portraits_tag.createFromFile(getResource(parser.getArg(1), Parser::EXT_IMAGE));
	}
	else if (parser.is("TIMER", 2)) {
		timer.createFromFile(getResource(parser.getArg(1), Parser::EXT_IMAGE));
		timer_font.createFromFile(getResource(parser.getArg(2), Parser::EXT_FONT));
	}
	else if (parser.is("SHINE", 1)) {
		shine.createFromFile(getResource(parser.getArg(1), Parser::EXT_IMAGE));
	}
	else if (parser.is("SHINE_TAG", 1)) {
		shine_tag.createFromFile(getResource(parser.getArg(1), Parser::EXT_IMAGE));
	}
	else if (parser.is("HP", 3)) {
		meterHp.img.createFromFile(getResource(parser.getArg(1), Parser::EXT_IMAGE));

		meterHp.pos.x = parser.getArgInt(2);
		meterHp.pos.y = parser.getArgInt(3);
	}
	else if (parser.is("SUPER", 3)) {
		meterSuper.img.createFromFile(getResource(parser.getArg(1), Parser::EXT_IMAGE));

		meterSuper.pos.x = parser.getArgInt(2);
		meterSuper.pos.y = parser.getArgInt(3);
	}
	else if (parser.is("TAG", 3)) {
		meterTag.img.createFromFile(getResource(parser.getArg(1), Parser::EXT_IMAGE));

		meterTag.pos.x = parser.getArgInt(2);
		meterTag.pos.y = parser.getArgInt(3);
	}
	else if (parser.is("STUN", 3)) {
		meterStun.img.createFromFile(getResource(parser.getArg(1), Parser::EXT_IMAGE));

		meterStun.pos.x = parser.getArgInt(2);
		meterStun.pos.y = parser.getArgInt(3);
	}
	else if (parser.is("GUARD", 3)) {
		meterGuard.img.createFromFile(getResource(parser.getArg(1), Parser::EXT_IMAGE));

		meterGuard.pos.x = parser.getArgInt(2);
		meterGuard.pos.y = parser.getArgInt(3);
	}
	else if (parser.is("DPM", 3)) {
		meterDpm.img.createFromFile(getResource(parser.getArg(1), Parser::EXT_IMAGE));

		meterDpm.pos.x = parser.getArgInt(2);
		meterDpm.pos.y = parser.getArgInt(3);
	}
	else if (parser.is("STATIC", 2)) {
		staticImg.createFromFile(getResource(parser.getArg(1), Parser::EXT_IMAGE));
		staticSnd.createFromFile(getResource(parser.getArg(2), Parser::EXT_SOUND));
	}
	else if (parser.is("FADE", 2)) {
		fadeinSnd.createFromFile(getResource(parser.getArg(1), Parser::EXT_SOUND));
		fadeoutSnd.createFromFile(getResource(parser.getArg(2), Parser::EXT_SOUND));
	}
	else if (parser.is("ROUND_SPLASH", 5)) {
		for (int i = 0; i < 5; i++) {
			round_splash[i].createFromFile(getResource(parser.getArg(i + 1), Parser::EXT_IMAGE));
		}
	}
	else if (parser.is("ROUND_HUD", 7)) {
		for (int i = 0; i < 5; i++) {
			round_hud[i].createFromFile(getResource(parser.getArg(i + 1), Parser::EXT_IMAGE));
		}
		x_round_hud = parser.getArgInt(6);
		y_round_hud = parser.getArgInt(7);
	}
	else if (parser.is("KO", 3)) {
		for (int i = 0; i < 3; i++) {
			ko[i].createFromFile(getResource(parser.getArg(i + 1), Parser::EXT_IMAGE));
		}
	}
	else if (parser.is("PORTRAIT_POS", 2)) {
		portraitPos.x = parser.getArgInt(1);
		portraitPos.y = parser.getArgInt(2);
	}
	else if (parser.is("COMBO", 3)) {
		combo.createFromFile(getResource(parser.getArg(1), Parser::EXT_FONT));
		comboLeft.createFromFile(getResource(parser.getArg(2), Parser::EXT_IMAGE));
		comboRight.createFromFile(getResource(parser.getArg(3), Parser::EXT_IMAGE));
	}
	else if (parser.is("WIN", 3)) {
		win.createFromFile(getResource(parser.getArg(1), Parser::EXT_IMAGE));
		win_font.createFromFile(getResource(parser.getArg(2), Parser::EXT_FONT));
		win_bgm.createFromFile("", getResource(parser.getArg(3), Parser::EXT_MUSIC));
	}
	else if (parser.is("WIN_ORBS", 5)) {
		orb_null.createFromFile(getResource(parser.getArg(1), Parser::EXT_IMAGE));
		orb_win.createFromFile(getResource(parser.getArg(2), Parser::EXT_IMAGE));
		orb_draw.createFromFile(getResource(parser.getArg(3), Parser::EXT_IMAGE));

		orb_pos.x = parser.getArgInt(4);
		orb_pos.y = parser.getArgInt(5);
	}
	else {
		Scene::parseLine(parser);
	}
}

void SceneFight::think() {
	if (frameShakeTimer) {
		cameraShake.x = (util::roll(frameShakeTimer * 2)) - frameShakeTimer;
		cameraShake.y = (util::roll(frameShakeTimer * 2)) - frameShakeTimer;
		frameShakeTimer--;
	}
	else {
		cameraShake.x = cameraShake.y = 0;
	}

	idealCameraPos.x = (madotsuki.pos.x + poniko.pos.x) / 2;
	idealCameraPos.y = (madotsuki.pos.y + poniko.pos.y) / 3 - 30;

	if (idealCameraPos.y < 0) {
		idealCameraPos.y = 0;
	}
	if (idealCameraPos.y > STAGE.heightAbs - sys::WINDOW_HEIGHT) {
		idealCameraPos.y = STAGE.heightAbs - sys::WINDOW_HEIGHT;
	}

	if (idealCameraPos.x < STAGE.widthAbs / -2 + sys::WINDOW_WIDTH / 2) {
		idealCameraPos.x = STAGE.widthAbs / -2 + sys::WINDOW_WIDTH / 2;
	}
	else if (idealCameraPos.x > STAGE.widthAbs / 2 - sys::WINDOW_WIDTH / 2) {
		idealCameraPos.x = STAGE.widthAbs / 2 - sys::WINDOW_WIDTH / 2;
	}

	cameraPos.x = (cameraPos.x * 0.8 + idealCameraPos.x * 0.2);
	cameraPos.y = (cameraPos.y * 0.8 + idealCameraPos.y * 0.2);

	cameraPos.x += cameraShake.x;
	cameraPos.y += cameraShake.y;

	if (madotsuki.pos.x < poniko.pos.x) {
		//if(madotsuki.inStandardState(STATE_STAND))
		if (madotsuki.isIdle()) {
			madotsuki.setDir(game::RIGHT);
		}
		//if(poniko.inStandardState(STATE_STAND))
		if (poniko.isIdle()) {
			poniko.setDir(game::LEFT);
		}
	}
	else if (madotsuki.pos.x > poniko.pos.x) {
		//if(madotsuki.inStandardState(STATE_STAND))
		if (madotsuki.isIdle()) {
			madotsuki.setDir(game::LEFT);
		}
		//if(poniko.inStandardState(STATE_STAND))
		if (poniko.isIdle()) {
			poniko.setDir(game::RIGHT);
		}
	}

	STAGE.think();

	madotsuki.think();
	poniko.think();

	if (!framePauseTimer) {

		madotsuki.interact(&poniko);
		poniko.interact(&madotsuki);

		for (int i = 0; i < game::MAX_PROJECTILES; i++) {
			if (madotsuki.projectiles[i].state != game::STATE_NONE) {
				madotsuki.projectiles[i].interact(&poniko);
			}
			if (poniko.projectiles[i].state != game::STATE_NONE) {
				poniko.projectiles[i].interact(&madotsuki);
			}
		}
	}
	else {
		framePauseTimer--;
	}

	Scene::think();

	if (gametype == GAMETYPE_TRAINING) {
		ko_player = 0;
		timer_ko = 0;
		timer_round_in = 0;
		timer_round_out = 0;
		game_timer = 0;

		if (!bgmPlaying) {
			STAGE.bgmPlay();
			bgmPlaying = true;
		}
	}

	if (winner) {
		if (input(game::INPUT_B)) {
			setScene(SCENE_SELECT);
		}
		else if (input(game::INPUT_A)) {
			reset();
		}
	}
	else {
		//Decrement timer
		if (game_timer && !timer_ko && !timer_round_in && !timer_round_out) {
			if (!--game_timer) {
				timer_ko = 1 * sys::FPS;
				ko_type = 1;
				if (madotsuki.hp == poniko.hp) {
					ko_player = 3;
				}
				else if (madotsuki.hp < poniko.hp) {
					ko_player = 1;
				}
				else {
					ko_player = 2;
				}

				if (madotsuki.flags & game::F_ON_GROUND && madotsuki.isDashing()) {
					if (madotsuki.inStandardState(game::STATE_DASH_FORWARD)) {
						madotsuki.setStandardState(game::STATE_DASH_FORWARD_END);
					}
					if (madotsuki.inStandardState(game::STATE_DASH_BACK)) {
						madotsuki.setStandardState(game::STATE_DASH_BACK_END);
					}
				}
				if (poniko.flags & game::F_ON_GROUND && poniko.isDashing()) {
					if (poniko.inStandardState(game::STATE_DASH_FORWARD)) {
						poniko.setStandardState(game::STATE_DASH_FORWARD_END);
					}
					if (poniko.inStandardState(game::STATE_DASH_BACK)) {
						poniko.setStandardState(game::STATE_DASH_BACK_END);
					}
				}
			}
		}

		//Combo counters
		//LEFT
		if (!ko_player) {
			if (madotsuki.comboCounter) {
				if (madotsuki.comboCounter == 1) {
					comboLeftOff = 0;
					comboLeftTimer = 0;
				}
				else if (madotsuki.comboCounter >= 2) {
					if (comboLeftLast < madotsuki.comboCounter) {
						comboLeftTimer = sys::FPS;
					}
					else if (comboLeftLast > madotsuki.comboCounter) {
						comboLeftOff = 0;
						comboLeftTimer = sys::FPS;
					}
				}
				comboLeftLast = madotsuki.comboCounter;
			}
		}

		if (comboLeftTimer) {
			comboLeftTimer--;
			if (comboLeftOff < comboLeft.w) {
				comboLeftOff += 16;
			}
			if (comboLeftOff > comboLeft.w) {
				comboLeftOff = comboLeft.w;
			}
		}
		else if (comboLeftOff && (madotsuki.comboCounter < 2 || ko_player)) {
			if (comboLeftOff > 0) {
				comboLeftOff -= 16;
			}
			if (comboLeftOff <= 0) {
				comboLeftOff = 0;
				comboLeftLast = 0;
			}
		}

		//RIGHT
		if (poniko.comboCounter > 1 && !ko_player) {
			if (comboRightLast < poniko.comboCounter) {
				comboRightTimer = sys::FPS;
			}
			else if (comboRightLast > poniko.comboCounter) {
				comboRightOff = 0;
				comboRightTimer = sys::FPS;
			}
			comboRightLast = poniko.comboCounter;
		}

		if (comboRightTimer) {
			comboRightTimer--;
			if (comboRightOff < comboRight.w) {
				comboRightOff += 16;
			}
			if (comboRightOff > comboRight.w) {
				comboRightOff = comboRight.w;
			}
		}
		else if (comboRightOff && (poniko.comboCounter < 2 || ko_player)) {
			if (comboRightOff > 0) {
				comboRightOff -= 16;
			}
			if (comboRightOff <= 0) {
				comboRightOff = 0;
				comboRightLast = 0;
			}
		}


		//ROUND INTROS
		if (!SceneOptions::optionEpilepsy && (timer_round_out || timer_round_in)) {
			if (!timer_flash && !util::roll(64)) {
				staticSnd.play();
				timer_flash = 5;
				staticImg.draw(0, 0);
			}
		}
		if (timer_round_in == (int)(4.0 * sys::FPS)) {
			madotsuki.reset();
			poniko.reset();
			cameraPos.x = 0;
			cameraPos.y = 0;
			idealCameraPos.x = 0;
			idealCameraPos.y = 0;
			fadeinSnd.play();
		}
		if (timer_round_out == (int)(1.5 * sys::FPS)) {
			fadeoutSnd.play();
		}

		if (timer_round_in == (int)(1.4 * sys::FPS) && !bgmPlaying) {
			STAGE.bgmPlay();
			bgmPlaying = true;
		}

		if (timer_round_in) {
			timer_round_in--;
		}
		if (timer_round_out) {
			timer_round_out--;
			if (timer_round_out == (int)(3.8 * sys::FPS)) {
				if (ko_player == 2) {
					if (!(poniko.flags & game::F_DEAD)) {
						poniko.setStandardState(game::STATE_DEFEAT);
					}
					madotsuki.setStandardState(game::STATE_VICTORY);
					win_types[0][wins[0]++] = 0;
				}
				else if (ko_player == 1) {
					if (!(madotsuki.flags & game::F_DEAD)) {
						madotsuki.setStandardState(game::STATE_DEFEAT);
					}
					poniko.setStandardState(game::STATE_VICTORY);
					win_types[1][wins[1]++] = 0;
				}
				else if (ko_player == 3) {
					if (ko_type != 2) {
						timer_round_out = 0;
						timer_ko = 1 * sys::FPS;
						ko_type = 2;
					}
					else {
						if (!(madotsuki.flags & game::F_DEAD)) {
							madotsuki.setStandardState(game::STATE_DEFEAT);
						}
						if (!(poniko.flags & game::F_DEAD)) {
							poniko.setStandardState(game::STATE_DEFEAT);
						}
						win_types[0][wins[0]++] = 1;
						win_types[1][wins[1]++] = 1;
						ko_type = 0;
					}
				}
			}
			else if (!timer_round_out) {
				//See if someone's won
				if (wins[0] >= SceneOptions::optionWins || wins[1] >= SceneOptions::optionWins) {
					//Count up the wins
					int wins_p1 = 0;
					int wins_p2 = 0;
					for (int i = 0; i < wins[0]; i++)
						if (win_types[0][i] == 0) {
							wins_p1++;
						}
					for (int i = 0; i < wins[1]; i++)
						if (win_types[1][i] == 0) {
							wins_p2++;
						}

					if (wins_p1 > wins_p2) {
						winner = 1;
					}
					else if (wins_p1 < wins_p2) {
						winner = 2;
					}
					else {
						winner = 3;
					}
				}

				if (winner) {
					win_bgm.play();
				}
				timer_round_in = 4.0 * sys::FPS;
				ko_player = 0;
				game_timer = SceneOptions::optionTime * sys::FPS - 1;
				if (game_timer < 0) {
					game_timer = 0;
				}
				round++;
			}
		}
		if (timer_ko) {
			timer_ko--;
		}

		if (ko_player && !timer_ko && !timer_round_out) {
			//Make sure everyone's still
			bool _condition = false;
			if (ko_type == 0) {
				if (ko_player == 2) {
					_condition = madotsuki.isIdle() && (madotsuki.flags & game::F_ON_GROUND) &&
						(poniko.inStandardState(game::STATE_PRONE) || poniko.inStandardState(game::STATE_ON_BACK));
				}
				else if (ko_player == 1) {
					_condition = poniko.isIdle() && (poniko.flags & game::F_ON_GROUND) &&
						(madotsuki.inStandardState(game::STATE_PRONE) || madotsuki.inStandardState(game::STATE_ON_BACK));
				}
			}
			else if (ko_type == 1) {
				_condition = poniko.isIdle() && (poniko.flags & game::F_ON_GROUND) && madotsuki.isIdle() && (madotsuki.flags & game::F_ON_GROUND);
			}
			else if (ko_type == 2) {
				_condition = true;
			}
			if (_condition) {
				timer_round_out = 4 * sys::FPS;
			}
		}
	}
}

void SceneFight::draw() const {
	// From main.cpp
	STAGE.draw(false);

	madotsuki.drawSpecial();
	poniko.drawSpecial();

	if (Stage::stage != 3) {
		madotsuki.draw(true);
		poniko.draw(true);
	}

	//Which order do we draw these in?
	if (madotsuki.drawPriorityFrame < poniko.drawPriorityFrame) {
		madotsuki.draw(false);
		poniko.draw(false);
	}
	else {
		poniko.draw(false);
		madotsuki.draw(false);
	}

	//Draw projectiles
	for (int i = 0; i < game::MAX_PROJECTILES; i++) {
		if (madotsuki.projectiles[i].state != game::STATE_NONE) {
			madotsuki.projectiles[i].draw();
		}
		if (poniko.projectiles[i].state != game::STATE_NONE) {
			poniko.projectiles[i].draw();
		}
	}

	STAGE.draw(true);

	effect::draw();

	Scene::draw();

	if (winner) {
		win.draw(0, 0);

		char _b_sz[256];
		if (winner == 3) {
			strcpy(_b_sz, "Draw!");
		}
		else {
			sprintf(_b_sz, "Player %d wins!", winner);
		}
		win_font.drawText(32, sys::FLIP(32), _b_sz);
	}
	else {
		hud.draw(0, 0);
		hud.draw(sys::WINDOW_WIDTH - hud.w, 0, true);

		//DRAW METERS

		meterHp.draw(madotsuki.hp / (float)madotsuki.getMaxHp(), false, false);
		meterHp.draw(poniko.hp / (float)poniko.getMaxHp(), true, false);

		meterSuper.draw(madotsuki.super / (float)game::SUPER_MAX, false, false);
		meterSuper.draw(poniko.super / (float)game::SUPER_MAX, true, false);

		//meterTag.draw(1, false, false);
		//meterTag.draw(1, true, false);

		meterStun.draw(1, false, false);
		meterStun.draw(1, true, false);

		meterGuard.draw(1, false, true);
		meterGuard.draw(1, true, true);

		meterDpm.draw(1, false, false);
		meterDpm.draw(1, true, false);

		portraits.draw(0, 0);
		portraits.draw(sys::WINDOW_WIDTH - shine.w, 0, true);

		timer.draw(0, 0);

		if (FIGHT->gametype != GAMETYPE_TRAINING) {
			//Round orbs
			for (int i = 0; i < SceneOptions::optionWins; i++) {
				int x = orb_pos.x - i * 18;
				if (i < wins[0]) {
					if (win_types[0][i]) {
						orb_draw.draw(x, orb_pos.y);
					}
					else {
						orb_win.draw(x, orb_pos.y);
					}
				}
				else {
					orb_null.draw(x, orb_pos.y);
				}

				if (i < wins[1]) {
					if (win_types[0][i]) {
						orb_draw.draw(sys::WINDOW_WIDTH - x - 18, orb_pos.y);
					}
					else {
						orb_win.draw(sys::WINDOW_WIDTH - x - 18, orb_pos.y);
					}
				}
				else {
					orb_null.draw(sys::WINDOW_WIDTH - x - 18, orb_pos.y);
				}
			}

			//Draw timer
			char b_timer_text[8];
			if (game_timer) {
				sprintf(b_timer_text, "%02d", (game_timer / sys::FPS) + 1);
			}
			else {
				strcpy(b_timer_text, "00");
			}
			int w_timer_text = timer_font.getTextWidth(b_timer_text);
			timer_font.drawText((sys::WINDOW_WIDTH - w_timer_text) / 2, 30, b_timer_text);
		}

		shine.draw(0, 0);
		shine.draw(sys::WINDOW_WIDTH - shine.w, 0, true);

		round_hud[round].draw(x_round_hud, y_round_hud);

		//Draw combo counters
		//LEFT
		if (comboLeftOff) {
			comboLeft.draw(comboLeftOff - comboLeft.w, 131);
			char buff[8];
			sprintf(buff, "%d", comboLeftLast);
			int w = combo.getTextWidth(buff);
			combo.drawText(comboLeftOff - w / 2 - 100, 131 + 35, buff);
		}

		//RIGHT
		if (comboRightOff) {
			comboRight.draw(sys::WINDOW_WIDTH - comboRightOff, 131);
			char buff[8];
			sprintf(buff, "%d", comboRightLast);
			int w = combo.getTextWidth(buff);
			combo.drawText(sys::WINDOW_WIDTH - comboRightOff - w / 2 + 100, 131 + 35, buff);
		}

		//Draw character portraits
		if (graphics::shader_support) {
			graphics::setPalette(madotsuki.fighter->palettes[madotsuki.palette], 1.0f, 0.0f, 0.0f, 0.0f, 0.0f);
		}
		madotsuki.fighter->portrait_ui.draw(portraitPos.x, portraitPos.y);
		if (graphics::shader_support) {
			graphics::setPalette(poniko.fighter->palettes[poniko.palette], 1.0f, 0.0f, 0.0f, 0.0f, 0.0f);
		}
		else {
			graphics::setColor(150, 150, 150, 1.0);
		}
		poniko.fighter->portrait_ui.draw(sys::WINDOW_WIDTH - portraitPos.x - poniko.fighter->portrait_ui.w, portraitPos.y, true);
		if (graphics::shader_support) {
			glUseProgram(0);
		}


		//Draw round transitions
		if (timer_flash) {
			graphics::setColor(180, 120, 190, timer_flash / 5.0f);
			staticImg.draw(-util::roll(sys::WINDOW_WIDTH), -util::roll(sys::WINDOW_HEIGHT));
			timer_flash--;
		}

		if (timer_round_out) {
			float alpha = 0.0f;
			if (timer_round_out < 0.5 * sys::FPS) {
				alpha = 1.0f;
			}
			else if (timer_round_out < 1.5 * sys::FPS) {
				alpha = 1.0 - ((timer_round_out - 0.5 * sys::FPS) / (1.0 * sys::FPS));
			}
			graphics::setColor(180, 120, 190, alpha);
			staticImg.draw(-util::roll(sys::WINDOW_WIDTH), -util::roll(sys::WINDOW_HEIGHT));
		}

		if (timer_round_in) {
			if (timer_round_in > 1.0 * sys::FPS) {
				graphics::setColor(180, 120, 190, ((timer_round_in - 1.0 * sys::FPS) / (3.0 * sys::FPS)));
				staticImg.draw(-util::roll(sys::WINDOW_WIDTH), -util::roll(sys::WINDOW_HEIGHT));
			}

			if (timer_round_in <= 1.4 * sys::FPS) {
				if (timer_round_in > 1.3 * sys::FPS) {
					float scalar = (timer_round_in - 1.3 * sys::FPS) / (0.1 * sys::FPS) + 1.0;
					graphics::setScale(scalar);
					round_splash[round].draw(sys::WINDOW_WIDTH / 2 - round_splash[round].w * scalar / 2 - util::roll(10, 30), sys::WINDOW_HEIGHT / 2 - round_splash[round].h * scalar / 2 - util::roll(10, 30));
				}
				else if (timer_round_in < 0.1 * sys::FPS) {
					float scalar = timer_round_in / (0.1 * sys::FPS);
					graphics::setColor(255, 255, 255, scalar);
					round_splash[round].draw(sys::WINDOW_WIDTH / 2 - round_splash[round].w / 2, sys::WINDOW_HEIGHT / 2 - round_splash[round].h / 2);
					graphics::setColor(255, 255, 255, scalar);
					scalar = 1.0f - scalar + 1.0f;
					float xscalar = 1 / scalar;
					graphics::setScale(xscalar, scalar);
					round_splash[round].draw(sys::WINDOW_WIDTH / 2 - round_splash[round].w * xscalar / 2 - util::roll(10, 30), sys::WINDOW_HEIGHT / 2 - round_splash[round].h * scalar / 2 - util::roll(10, 30));
				}
				else {
					round_splash[round].draw(sys::WINDOW_WIDTH / 2 - round_splash[round].w / 2 - util::roll(5, 15), sys::WINDOW_HEIGHT / 2 - round_splash[round].h / 2 - util::roll(5, 15));
				}
			}
		}

		if (timer_ko) {
			if (timer_ko > 0.8 * sys::FPS) {
				float scalar = (timer_ko - 0.8 * sys::FPS) / (0.1 * sys::FPS) + 1.0;
				graphics::setScale(scalar);
				ko[ko_type].draw(sys::WINDOW_WIDTH / 2 - ko[ko_type].w * scalar / 2 - util::roll(10, 30), sys::WINDOW_HEIGHT / 2 - ko[ko_type].h * scalar / 2 - util::roll(10, 30));
			}
			else if (timer_ko < 0.1 * sys::FPS) {
				float scalar = timer_ko / (0.1 * sys::FPS);
				graphics::setColor(255, 255, 255, scalar);
				ko[ko_type].draw(sys::WINDOW_WIDTH / 2 - ko[ko_type].w / 2, sys::WINDOW_HEIGHT / 2 - ko[ko_type].h / 2);
				graphics::setColor(255, 255, 255, scalar);
				scalar = 1.0f - scalar + 1.0f;
				float xscalar = 1 / scalar;
				graphics::setScale(xscalar, scalar);
				ko[ko_type].draw(sys::WINDOW_WIDTH / 2 - ko[ko_type].w * xscalar / 2 - util::roll(10, 30), sys::WINDOW_HEIGHT / 2 - ko[ko_type].h * scalar / 2 - util::roll(10, 30));
			}
			else {
				ko[ko_type].draw(sys::WINDOW_WIDTH / 2 - ko[ko_type].w / 2 - util::roll(5, 15), sys::WINDOW_HEIGHT / 2 - ko[ko_type].h / 2 - util::roll(5, 15));
			}
		}
	}

	// From main.cpp
	((SceneSelect*)scenes[SCENE_SELECT].get())->drawEffect(0, madotsuki.fighter->group, madotsuki.pos.x, madotsuki.pos.y + madotsuki.fighter->height, true);
}

void SceneFight::reset() {
	ko_player = 0;
	ko_type = 0;

	timer_flash = 0;
	timer_round_in = 4.0 * sys::FPS;
	timer_round_out = 0;
	timer_ko = 0;

	round = 0;
	wins[0] = 0;
	wins[1] = 0;
	winner = 0;

	comboLeftOff = comboRightOff = 0;
	comboLeftLast = comboRightLast = 0;
	comboLeftTimer = comboRightTimer = 0;

	game_timer = SceneOptions::optionTime * sys::FPS - 1;
	if (game_timer < 0) {
		game_timer = 0;
	}

	bgmPlaying = false;
}

void SceneFight::knockout(int player) {
	if (ko_player) {
		return;
	}
	ko_player = player + 1;
	timer_ko = 1 * sys::FPS;
	ko_type = 0;

	if (madotsuki.flags & game::F_ON_GROUND && madotsuki.isDashing()) {
		if (madotsuki.inStandardState(game::STATE_DASH_FORWARD)) {
			madotsuki.setStandardState(game::STATE_DASH_FORWARD_END);
		}
		if (madotsuki.inStandardState(game::STATE_DASH_BACK)) {
			madotsuki.setStandardState(game::STATE_DASH_BACK_END);
		}
	}
	if (poniko.flags & game::F_ON_GROUND && poniko.isDashing()) {
		if (poniko.inStandardState(game::STATE_DASH_FORWARD)) {
			poniko.setStandardState(game::STATE_DASH_FORWARD_END);
		}
		if (poniko.inStandardState(game::STATE_DASH_BACK)) {
			poniko.setStandardState(game::STATE_DASH_BACK_END);
		}
	}
}