#include "versus.h"

#include "scene.h"

#include "../sys.h"
#include "../graphics.h"

//Scene Versus
SceneVersus::SceneVersus() : Scene("versus") {
	//video = nullptr;

	timer1 = timer2 = timer3 = timer4 = timer5 = timer6 = 0;
	timerF = 0.0f;

	portraits[0] = portraits[1] = nullptr;
}

SceneVersus::~SceneVersus() {
}

void SceneVersus::init() {
	Scene::init();
}

void SceneVersus::think() {
	Scene::think();

	if (input(game::INPUT_A))
		setScene(SCENE_FIGHT);

	if (timer1 > 0) {
		timer1--;
	}
	else if (timer2 > 0) {
		timer2 *= 0.90;
	}
	else if (timer3 > 0) {
		timer3--;
	}
	else if (timer4 > 0) {
		timer4 *= 0.90;
	}
	else if (timer5 > 0) {
		timer5--;
	}
	else if (timer6 > 0) {
		timer6--;
		if (!timer6) {
			setScene(SCENE_FIGHT);
		}
	}


	if (!timer5) {
		timer2 -= 100;
	}

	if (!timer1) {
		timerF += 0.03f;
	}
}

void SceneVersus::reset() {
	Scene::reset();

	timer1 = sys::FPS * 2;
	timer2 = 1000;
	timer3 = sys::FPS * 1;
	timer4 = 1000;
	timer5 = sys::FPS * 2;
	timer6 = sys::FPS * 1.5;

	timerF = 0.0f;
}

void SceneVersus::draw() const {
	Scene::draw();

	if (portraits[1]) {
		graphics::setColor(255, 255, 255, 0.5f);// MIN(1.0f, (FPS * 2 + FPS / 2 - timer6) / 60.0f));
		graphics::setScale(2.0f);
		portraits[1]->draw(sys::WINDOW_WIDTH - portraits[1]->w * 1.5 - timer2 + timerF, -120, true);
		portraits[1]->draw(sys::WINDOW_WIDTH - portraits[1]->w + timer4 - timerF, 0, true);
	}

	if (portraits[0]) {
		graphics::setColor(255, 255, 255, 0.5f);// MIN(1.0f, (FPS * 2 + FPS / 2 - timer6) / 60.0f));
		graphics::setScale(2.0f);
		portraits[0]->draw(portraits[0]->w * -0.5 + timer2 - timerF, -120);
		portraits[0]->draw(0 - timer4 + timerF, 0);
	}
}

void SceneVersus::parseLine(Parser& parser) {
	Scene::parseLine(parser);
}
