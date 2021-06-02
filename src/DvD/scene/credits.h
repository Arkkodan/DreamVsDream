#ifndef DVD_SCENE_CREDITS_H
#define DVD_SCENE_CREDITS_H

#include "scene_base.h"

#include "../font.h"

/// @brief In-game credits
class SceneCredits : public Scene {
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

	ubyte_t title_r;
	ubyte_t title_g;
	ubyte_t title_b;

	ubyte_t name_r;
	ubyte_t name_g;
	ubyte_t name_b;

	void think();
	void draw();
	void reset();
	void parseLine(Parser& parser);
};

#endif // DVD_SCENE_CREDITS_H
