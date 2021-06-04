#ifndef DVD_SCENE_VERSUS_H
#define DVD_SCENE_VERSUS_H

#include "scene_base.h"

#include <array>

/// @brief Versus splash cutscene
class SceneVersus : public Scene {
public:
	SceneVersus();
	~SceneVersus();

	void init() override final;

	//Members
	std::array<Image*, 2> portraits;
	int timer1;
	int timer2;
	int timer3;
	int timer4;
	int timer5;
	int timer6;

	float timerF;

	//Functions
	void think() override final;
	void reset() override final;
	void draw() const override final;

	void parseLine(Parser& parser) override final;
};

#endif // DVD_SCENE_VERSUS_H
