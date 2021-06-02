#ifndef DVD_SCENE_FIGHT_H
#define DVD_SCENE_FIGHT_H

#include "scene_base.h"

#include "../util.h"
#include "../font.h"

/// @brief Helper object for drawing meters, primarily for SceneFight
class SceneMeter {
public:
	SceneMeter();
	~SceneMeter();

	void draw(float pct, bool mirror, bool flip);

	Image img;
	util::Vector pos;
};

/// @brief Fighting game scene
class SceneFight : public Scene {
public:
	enum {
		GAMETYPE_VERSUS,
		GAMETYPE_TRAINING,
	};

public:
	SceneFight();
	~SceneFight();

	void init();

	Image hud;
	Image hud_tag;
	Image portraits;
	Image portraits_tag;
	Image timer;
	Font timer_font;
	Image shine;
	Image shine_tag;

	//Bars and stuff
	SceneMeter meterHp;
	SceneMeter meterSuper;
	SceneMeter meterTag;
	SceneMeter meterStun;
	SceneMeter meterGuard;
	SceneMeter meterDpm;

	Image staticImg;
	audio::Sound staticSnd;
	audio::Sound fadeinSnd;
	audio::Sound fadeoutSnd;

	Image round_splash[5];
	Image round_hud[5];
	int x_round_hud;
	int y_round_hud;

	Image ko[3]; //KO, timeout, draw

	Font combo;
	Image comboLeft;
	Image comboRight;
	unsigned int comboLeftOff;
	unsigned int comboRightOff;
	int comboLeftLast;
	int comboRightLast;
	int comboLeftTimer;
	int comboRightTimer;

	util::Vector portraitPos;

	Image win;
	Font win_font;
	audio::Music win_bgm;

	Image orb_null;
	Image orb_win;
	Image orb_draw;
	util::Vector orb_pos;

	//Timers for various effects
	int timer_flash;
	int timer_round_in;
	int timer_round_out;
	int timer_ko; //timeout/draw too

	//Logic
	int round; //What round is it?
	int wins[2]; //How many times each player has on (including draws)
	int win_types[2][3]; //What type of win was it? Win or draw?
	int winner; //Who won the game?

	int ko_player; //Which player was ko'd/lost? 2 = draw
	int ko_type;   //Was it a ko or was it a timeout?

	int game_timer; //this is the actual game timer

	int gametype;

	//Functions
	void think();
	void draw();
	void reset();

	void knockout(int player);

	void parseLine(Parser& parser);
};

#endif // DVD_SCENE_FIGHT_H
