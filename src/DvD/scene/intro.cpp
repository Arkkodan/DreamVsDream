#include "intro.h"

#include "scene.h"

SceneIntro::SceneIntro() : Scene("intro") {
	timer = FPS / 2;
	state = 0;
}

SceneIntro::~SceneIntro() {
}

void SceneIntro::think() {
	Scene::think();

	if (timer == FPS / 2) {
		sfx.play();
	}

	if (input(INPUT_A) && !timer) {
		timer = FPS / 2;
		state++;
	}

	if (timer) {
		timer--;
		if (!timer && state % 2 == 1) {
			timer = FPS / 2;
			state++;
			if (state == 6 && graphics::shader_support) {
				timer = 0;
				setScene(SCENE_TITLE);
			}
			if (state == 8) {
				timer = 0;
				setScene(SCENE_TITLE);
			}
		}
	}
}

void SceneIntro::draw() {
	//Draw our own fade
	float _alpha = timer / (float)(FPS / 2);
	if (state % 2 == 0) {
		_alpha = 1.0 - _alpha;
	}
	graphics::setColor(255, 255, 255, _alpha);
	if (state < 2) {
		graphics::setColor(255, 255, 255, _alpha);
		instructions.draw(0, 0);
	}
	else if (state < 4) {
		graphics::setColor(255, 255, 255, _alpha);
		disclaimer_en.draw(0, 0);
	}
	else if (state < 6) {
		graphics::setColor(255, 255, 255, _alpha);
		disclaimer_ja.draw(0, 0);
	}
	else if (state < 8 && !graphics::shader_support) {
		graphics::setColor(255, 255, 255, _alpha);
		shader_error.draw(0, 0);
	}
}

void SceneIntro::parseLine(Parser& parser) {
	if (parser.is("SFX", 1)) {
		sfx.createFromFile(getResource(parser.getArg(1), EXT_SOUND));
	}
	else if (parser.is("INSTRUCTIONS", 1)) {
		instructions.createFromFile(getResource(parser.getArg(1), EXT_IMAGE));
	}
	else if (parser.is("DISCLAIMER", 2)) {
		disclaimer_en.createFromFile(getResource(parser.getArg(1), EXT_IMAGE));
		disclaimer_ja.createFromFile(getResource(parser.getArg(2), EXT_IMAGE));
	}
	else if (parser.is("SHADER_ERROR", 1)) {
		shader_error.createFromFile(getResource(parser.getArg(1), EXT_IMAGE));
	}
	else {
		Scene::parseLine(parser);
	}
}