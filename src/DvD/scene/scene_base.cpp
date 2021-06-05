#include "scene_base.h"

#include "scene.h"

#include "../stage.h"
#include "../error.h"
#include "../sys.h"
#include "../../util/fileIO.h"
#include "../graphics.h"

#include <algorithm>

#include <glad/glad.h>

std::array<std::unique_ptr<Scene>, Scene::SCENE_MAX> Scene::scenes;
int Scene::scene = SCENE_INTRO;
int Scene::sceneNew = 0;

Image Scene::imgLoading;

void Scene::ginit() {
	scenes[SCENE_INTRO] = std::make_unique<SceneIntro>();
	scenes[SCENE_TITLE] = std::make_unique<SceneTitle>();
	scenes[SCENE_SELECT] = std::make_unique<SceneSelect>();
	scenes[SCENE_VERSUS] = std::make_unique<SceneVersus>();
	scenes[SCENE_FIGHT] = std::make_unique<SceneFight>();
	scenes[SCENE_OPTIONS] = std::make_unique<SceneOptions>();
#ifndef NO_NETWORK
	scenes[SCENE_NETPLAY] = std::make_unique<SceneNetplay>();
#endif
	scenes[SCENE_CREDITS] = std::make_unique<SceneCredits>();

	scenes[SCENE_FIGHT]->init();
	scenes[SCENE_VERSUS]->init();
	scenes[SCENE_INTRO]->init();
}

void Scene::deinit() {}

void Scene::setScene(int _scene) {
	if (_scene == scene) {
		return;
	}
	if (_scene == SCENE_FIGHT) {
		SceneFight::madotsuki.reset();
		SceneFight::poniko.reset();
		SceneFight::cameraPos.x = 0;
		SceneFight::cameraPos.y = 0;
		SceneFight::idealCameraPos.x = 0;
		SceneFight::idealCameraPos.y = 0;
	}
	sceneNew = _scene;
	fade = 1.0f;
	fadeIn = false;
}

bool Scene::input(uint16_t in) {
	return (SceneFight::madotsuki.frameInput & in) || (SceneFight::poniko.frameInput & in);
}

float Scene::fade = 1.0f;
bool Scene::fadeIn = true;

void Scene::drawFade() {
	//Draw fade!
	glBindTexture(GL_TEXTURE_2D, 0);
	if (fadeIn) {
		glColor4f(0.0f, 0.0f, 0.0f, fade);
	}
	else {
		glColor4f(0.0f, 0.0f, 0.0f, 1.0f - fade);
	}
	glBegin(GL_QUADS);
	glVertex3f(0.0f, 0.0f, 0.0f);
	glVertex3f(0.0f, sys::WINDOW_HEIGHT, 0.0f);
	glVertex3f(sys::WINDOW_WIDTH, sys::WINDOW_HEIGHT, 0.0f);
	glVertex3f(sys::WINDOW_WIDTH, 0.0f, 0.0f);
	glEnd();
	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
}

//SCENES
Scene::Scene(std::string name_) {
	//Copy the name.
	name = name_;

	initialized = false;
	bgmPlaying = false;
	//video = nullptr;
}

Scene::~Scene() {
	//delete video;
}

void Scene::init() {
	initialized = true;
	//Load the scene data from the file
	parseFile("scenes/" + name + ".ubu");
}

void Scene::think() {
	if (!initialized) {
		init();
	}

	if (!bgmPlaying &&
#ifndef NO_NETWORK
		scene != SCENE_NETPLAY &&
#endif
		scene != SCENE_FIGHT) {
		if (bgm.exists()) {
			bgm.play();
		}
		bgmPlaying = true;
	}
	std::for_each(images.begin(), images.end(),
		[](SceneImage& si) {si.think(); }
	);
	//if(video) video->think();

	//Fade timer
	if (fade) {
		//if(fadeIn && scene == SCENE_VERSUS) {
		//	fade = 0.0f;
		//} else {
		if (scene == SCENE_FIGHT) {
			fade -= 0.02;
		}
		else {
			fade -= 0.1f;
		}
		if (fade <= 0.0f) {
			if (fadeIn) {
				fade = 0.0f;
			}
			else {
				fade = 1.0f;
				fadeIn = true;

				//Are we quitting?
				if (sceneNew == SCENE_QUIT) {
					exit(0);
				}
				scene = sceneNew;

				audio::Music::stop();

				if (!SCENE->initialized) {
					//Loading graphic
					imgLoading.draw(0, 0);
					sys::refresh();
					SCENE->init();
				}
				if (scene == SCENE_FIGHT && !STAGE.initialized) {
					//Loading graphic
					imgLoading.draw(0, 0);
					sys::refresh();
					STAGE.init();
				}
				SCENE->reset();
			}
		}

		//Always disable controls during fades
		SceneFight::madotsuki.frameInput = 0;
		//}
	}
}

void Scene::reset() {
	std::for_each(images.begin(), images.end(),
		[](SceneImage& si) {si.reset(); }
	);
	//if(video) video->reset();
	bgmPlaying = false;
}

void Scene::draw() const {
	//if(video) video->draw(0, 0);
	std::for_each(images.cbegin(), images.cend(),
		[](const SceneImage& si) {si.draw(false); }
	);


}

void Scene::parseLine(Parser& parser) {
	int argc = parser.getArgC();
	if (parser.is("IMAGE", 3)) {
		float x = parser.getArgFloat(2);
		float y = parser.getArgFloat(3);
		Image::Render render = Image::Render::NORMAL;
		float xvel = 0.0f;
		float yvel = 0.0f;
		bool wrap = false;
		if (argc > 4) {
			std::string szRender = parser.getArg(4);
			if (!szRender.compare("additive")) {
				render = Image::Render::ADDITIVE;
			}
			else if (!szRender.compare("subtractive")) {
				render = Image::Render::SUBTRACTIVE;
			}
			else if (!szRender.compare("multiply")) {
				render = Image::Render::MULTIPLY;
			}
			if (argc > 5) {
				xvel = parser.getArgFloat(5);
				if (argc > 6) {
					yvel = parser.getArgFloat(6);
					if (argc > 7) {
						wrap = parser.getArgBool(7, false);
					}
				}
			}
		}

		//Add a new image
		Image imgData;
		imgData.createFromFile(getResource(parser.getArg(1), Parser::EXT_IMAGE));
		if (!imgData.exists()) {
			return;
		}
		images.emplace_back(imgData, x, y, 1.0f, render, xvel, yvel, wrap, 0);
	}
	else if (parser.is("BGM", 1)) {
		if (argc > 2) {
			std::string intro = getResource(parser.getArg(1), Parser::EXT_MUSIC);
			std::string loop = getResource(parser.getArg(2), Parser::EXT_MUSIC);
			bgm.createFromFile(intro, loop);
		}
		else {
			bgm.createFromFile("", getResource(parser.getArg(1), Parser::EXT_MUSIC));
		}
	}
	else if (parser.is("SOUND", 4)) {
		sndMenu.createFromFile(getResource(parser.getArg(1), Parser::EXT_SOUND));
		sndSelect.createFromFile(getResource(parser.getArg(2), Parser::EXT_SOUND));
		sndBack.createFromFile(getResource(parser.getArg(3), Parser::EXT_SOUND));
		sndInvalid.createFromFile(getResource(parser.getArg(4), Parser::EXT_SOUND));
	}
	else if (parser.is("VIDEO", 1)) {
		//getResource(parser.getArg(1), EXT_VIDEO);
	}
}

void Scene::parseFile(std::string szFileName) {
	Parser parser;
	std::string path = util::getPath(szFileName);
	if (!parser.open(path)) {
		error::die("Cannot parse file \"" + path + "\"");
	}

	//Get all the data
	while (parser.parseLine()) {
		//Parse it
		parseLine(parser);
	}
}

std::string Scene::getResource(std::string resource, std::string extension) const {
	if (*resource.c_str() == '*') {
		return "scenes/common/" + resource.substr(1, std::string::npos) + "." + extension;
	}
	else {
		return "scenes/" + name + "/" + resource + "." + extension;
	}
}

//SCENE IMAGE

SceneImage::SceneImage(Image& image_, float x_, float y_, float parallax_, Image::Render render_, float xvel_, float yvel_, bool wrap_, int round_) {
	image = std::move(image_);
	x = x_;
	y = y_;
	parallax = parallax_;
	xOrig = x_;
	yOrig = y_;
	render = render_;
	xvel = xvel_;
	yvel = yvel_;
	wrap = wrap_;
	round = round_;

	if (wrap) {
		while (x < 0.0f - image.w) {
			x += image.w;
		}
		while (y < 0.0f - image.h) {
			y += image.h;
		}
		while (x >= 0.0f) {
			x -= image.w;
		}
		while (y >= 0.0f) {
			y -= image.h;
		}
	}
}

SceneImage::~SceneImage() {}

void SceneImage::think() {
	x += xvel;
	y += yvel;

	//Wrap the wrapping images
	if (wrap) {
		while (x < 0.0f - image.w) {
			x += image.w;
		}
		while (y < 0.0f - image.h) {
			y += image.h;
		}
		while (x >= 0.0f) {
			x -= image.w;
		}
		while (y >= 0.0f) {
			y -= image.h;
		}
	}
}

void SceneImage::reset() {
	x = xOrig;
	y = yOrig;
}

void SceneImage::draw(bool _stage) const {
	if (image.exists()) {
		//Draw the image differently if wrapping
		if (wrap) {
			//How many of these are needed to fill the screen?
			int xCount = sys::WINDOW_WIDTH / image.w + 2;
			int yCount = sys::WINDOW_HEIGHT / image.h + 2;

			for (int i = 0; i < xCount; i++) {
				for (int j = 0; j < yCount; j++) {
					graphics::setRender(render);
					image.draw((int)x + i * image.w, (int)y + j * image.h);
				}
			}
		}
		else {
			graphics::setRender(render);
			if (_stage) {
				if (!round || round - 1 == FIGHT->round) {
					image.draw(x - image.w / 2 + sys::WINDOW_WIDTH / 2 - SceneFight::cameraPos.x * parallax, (sys::WINDOW_HEIGHT - y) - image.h + SceneFight::cameraPos.y * parallax);
				}
			}
			else {
				image.draw(x, y);
			}
		}
	}
}
