#ifndef DVD_SCENE_SELECT_H
#define DVD_SCENE_SELECT_H

#include "scene_base.h"

#include "../util.h"
#include "../font.h"

#include <vector>
#include <array>

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

	CursorData(const CursorData& other) = delete;
	CursorData& operator=(const CursorData& other) = delete;

	CursorData(CursorData&& other) noexcept;
	CursorData& operator=(CursorData& other) noexcept;
};

/// @brief Helper object for SceneSelect
class Cursor {
public:
	enum {
		CURSOR_UNLOCKED,
		CURSOR_COLORSWAP,
		CURSOR_LOCKED,
	};

public:
	int pos;
	int posOld;
	int posDefault;

	mutable int frame;
	mutable int timer;

	int timerPortrait;

	int lockState;

	uint8_t r;
	uint8_t g;
	uint8_t b;

	//int sprFrame;
	//int sprTimer;

	Cursor();

	int getGroup(int w, int gW, int gH) const;
};

/// @brief Character and stage selection
class SceneSelect : public Scene {
private:
	static constexpr auto PORTRAIT_FADE = 50;

public:
	SceneSelect();
	~SceneSelect();

	void init() override final;

	//Members
	int width, height;
	int gWidth, gHeight;
	//Image* sprites;
	std::list<SceneImage> gui;
	std::vector<util::Vector> grid;
	std::vector<int> gridFighters;
	int gridC;

	std::array<Cursor, 2> cursors;

	std::vector<CursorData> curData;

	Font font_stage;
	int cursor_stage;
	mutable float cursor_stage_offset;

	//Functions
	void think() override final;
	void reset() override final;
	void draw() const override final;

	void newEffect(int player, int group);
	void drawEffect(int player, int group, int _x, int _y, bool spr = false) const;

	void parseLine(Parser& parser) override final;
};

#endif // DVD_SCENE_SELECT_H