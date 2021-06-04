#ifndef DVD_SCENE_INTRO_H
#define DVD_SCENE_INTRO_H

#include "scene_base.h"

/// @brief Scene for the introductory sceen
class SceneIntro : public Scene {
public:
	SceneIntro();
	~SceneIntro();

	audio::Sound sfx;
	Image instructions;
	Image disclaimer_en;
	Image disclaimer_ja;
	Image shader_error;

	int timer;
	int state;

	void think() override final;
	void draw() const override final;

	void parseLine(Parser& parser) override final;
};

#endif // DVD_SCENE_INTRO_H
