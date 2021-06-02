#ifndef DVD_SCENE_OPTIONS_H
#define DVD_SCENE_OPTIONS_H

#include "scene_base.h"

#include "../font.h"

/// @brief Scene for modifying options
class SceneOptions : public Scene {
public:
	SceneOptions();
	~SceneOptions();

	void init();

	//Members
	Font menuFont;

	ubyte_t cursor;
	ubyte_t cursorLast;
	int cursorTimer;

	int madoPos;
	int madoDir;
	int madoFrame;

	Image madoImg;
	audio::Sound madoSfxStep;
	audio::Sound madoSfxPinch;

	audio::Voice dame;
	audio::Voice muri;

	int madoWakeTimer;

	ubyte_t iR, iG, iB, aR, aG, aB;
	int aXOffset;

	std::string* themes;
	int nThemes;

	//Functions
	void think();
	void reset();
	void draw();

	void parseLine(Parser& parser);
};

#endif // DVD_SCENE_OPTIONS_H
