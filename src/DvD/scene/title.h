#ifndef DVD_SCENE_TITLE_H
#define DVD_SCENE_TITLE_H

#include "scene_base.h"

#include "../font.h"

/// @brief Scene for the title menu
class SceneTitle : public Scene {
public:
	SceneTitle();
	~SceneTitle();

	void init();

	std::string* themes;
	int nThemes;

	//Members
	int menuX, menuY;
	Font menuFont;
	int menuXOffset;

	ubyte_t iR, iG, iB, aR, aG, aB;
	int aXOffset;

	int choiceTimer;
	ubyte_t choiceLast;
	ubyte_t choice;
	ubyte_t submenu;

	//Functions
	void think();
	void reset();
	void draw();

	void parseLine(Parser& parser);
};

#endif // DVD_SCENE_TITLE_H
