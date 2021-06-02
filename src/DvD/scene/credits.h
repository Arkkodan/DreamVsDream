#ifndef DVD_SCENE_CREDITS_H
#define DVD_SCENE_CREDITS_H

#include "scene_base.h"

#include "../font.h"

/// @brief In-game credits
class SceneCredits : public Scene {
private:
	static constexpr auto CREDITS_OFFSET = 20;

public:

	SceneCredits();
	~SceneCredits();

	//Logic
	bool done;
	float oy;
	int timer_start;
	int timer_scroll;
	float secret_alpha;

	//Data
	Image logo;
	Font font;
	int c_lines;
	char** sz_lines;

	uint8_t title_r;
	uint8_t title_g;
	uint8_t title_b;

	uint8_t name_r;
	uint8_t name_g;
	uint8_t name_b;

	void think();
	void draw();
	void reset();
	void parseLine(Parser& parser);
};

#endif // DVD_SCENE_CREDITS_H
