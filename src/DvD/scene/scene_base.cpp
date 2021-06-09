#include "scene_base.h"

#include "scene.h"
#include "fight.h"

#include "../stage.h"
#include "../error.h"
#include "../sys.h"
#include "../../util/fileIO.h"
#include "../graphics.h"

#include <algorithm>

#include <glad/glad.h>

//SCENES
scene::Scene::Scene(std::string name_) {
	//Copy the name.
	name = name_;

	initialized = false;
	bgmPlaying = false;
	//video = nullptr;
}

scene::Scene::~Scene() {
	//delete video;
}

void scene::Scene::init() {
	initialized = true;
	//Load the scene data from the file
	parseFile("scenes/" + name + ".ubu");
}

void scene::Scene::think() {
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
				if (scene == SCENE_FIGHT && !STAGE->initialized) {
					//Loading graphic
					imgLoading.draw(0, 0);
					sys::refresh();
					STAGE->init();
				}
				SCENE->reset();
			}
		}

		//Always disable controls during fades
		Fight::madotsuki.frameInput = 0;
		//}
	}
}

void scene::Scene::reset() {
	std::for_each(images.begin(), images.end(),
		[](SceneImage& si) {si.reset(); }
	);
	//if(video) video->reset();
	bgmPlaying = false;
}

void scene::Scene::draw() const {
	//if(video) video->draw(0, 0);
	std::for_each(images.cbegin(), images.cend(),
		[](const SceneImage& si) {si.draw(false); }
	);


}

void scene::Scene::parseLine(Parser& parser) {
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

void scene::Scene::parseFile(std::string szFileName) {
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

std::string scene::Scene::getResource(std::string resource, std::string extension) const {
	if (*resource.c_str() == '*') {
		return "scenes/common/" + resource.substr(1, std::string::npos) + "." + extension;
	}
	else {
		return "scenes/" + name + "/" + resource + "." + extension;
	}
}

//SCENE IMAGE

scene::SceneImage::SceneImage(Image& image_, float x_, float y_, float parallax_, Image::Render render_, float xvel_, float yvel_, bool wrap_, int round_) {
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

scene::SceneImage::~SceneImage() {}

void scene::SceneImage::think() {
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

void scene::SceneImage::reset() {
	x = xOrig;
	y = yOrig;
}

void scene::SceneImage::draw(bool _stage) const {
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
					image.draw(x - image.w / 2 + sys::WINDOW_WIDTH / 2 - Fight::cameraPos.x * parallax, (sys::WINDOW_HEIGHT - y) - image.h + Fight::cameraPos.y * parallax);
				}
			}
			else {
				image.draw(x, y);
			}
		}
	}
}
