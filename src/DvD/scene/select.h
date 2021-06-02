#ifndef DVD_SCENE_SELECT_H
#define DVD_SCENE_SELECT_H

#include "scene_base.h"

#include "../util.h"
#include "../font.h"

/// @brief Helper data structure for SceneSelect
class CursorData {
public:
	util::Vector off;
	Image img;
	Image imgSelect;
	audio::Sound sndSelect;
	audio::Sound sndDeselect;

	int frameC;
	int speed;
	bool grow;

	CursorData();
	~CursorData();
};

enum {
	CURSOR_UNLOCKED,
	CURSOR_COLORSWAP,
	CURSOR_LOCKED,
};

/// @brief Helper object for SceneSelect
class Cursor {
public:
	int pos;
	int posOld;
	int posDefault;

	int frame;
	int timer;

	int timerPortrait;

	int lockState;

	ubyte_t r;
	ubyte_t g;
	ubyte_t b;

	//int sprFrame;
	//int sprTimer;

	Cursor();

	int getGroup(int w, int gW, int gH);
};

/// @brief Character and stage selection
class SceneSelect : public Scene {
public:
	SceneSelect();
	~SceneSelect();

	void init();

	//Members
	int width, height;
	int gWidth, gHeight;
	//Image* sprites;
	SceneImage* gui;
	util::Vector* grid;
	int* gridFighters;
	int gridC;

	Cursor cursors[2];

	CursorData* curData;

	Font font_stage;
	int cursor_stage;
	float cursor_stage_offset;

	//Functions
	void think();
	void reset();
	void draw();

	void newEffect(int player, int group);
	void drawEffect(int player, int group, int _x, int _y, bool spr = false);

	void parseLine(Parser& parser);
};

#endif // DVD_SCENE_SELECT_H
