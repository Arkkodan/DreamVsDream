#ifndef DVD_SCENE_INTRO_H
#define DVD_SCENE_INTRO_H

#include "scene_base.h"

namespace scene {

	/// @brief Scene for the introductory sceen
	class Intro : public Scene {
	public:
		Intro();
		~Intro();

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
}

#endif // DVD_SCENE_INTRO_H
