#ifndef DVD_SCENE_VERSUS_H
#define DVD_SCENE_VERSUS_H

#include "scene_base.h"

/// @brief Versus splash cutscene
class SceneVersus : public Scene {
public:
	SceneVersus();
	~SceneVersus();

	void init();

	//Members
	Image* portraits[2];
	int timer1;
	int timer2;
	int timer3;
	int timer4;
	int timer5;
	int timer6;

	float timerF;

	//Functions
	void think();
	void reset();
	void draw();

	void parseLine(Parser& parser);
};

#endif // DVD_SCENE_VERSUS_H
