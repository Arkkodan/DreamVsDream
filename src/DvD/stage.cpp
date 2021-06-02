#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "stage.h"
#include "parser.h"
#include "graphics.h"
#include "error.h"

int stage = -1;
Stage stages[20];

void Stage::ginit() {
	stages[0].create("yn_balcony");
	stages[1].create("yn_block");
	stages[2].create("yn_desert");
	stages[3].create("yn_dungeon");
	stages[4].create("yn_garden");
	stages[5].create("yn_masada");
	stages[6].create("yn_numbers");
	stages[7].create("yn_poniko");
	stages[8].create("yn_sewers");
	stages[9].create("yn_wilderness");
	stages[10].create("flow_child");
	stages[11].create("flow_helltech");
	stages[12].create("flow_hotel");
	stages[13].create("flow_plant");
	stages[14].create("flow_rainbow");
	stages[15].create("flow_rot");
	stages[16].create("flow_school");
	stages[17].create("flow_sugar");
	stages[18].create("flow_underwater");
	stages[19].create("flow_white");

	//stages[0].create("y2_mushroom");
}

void Stage::deinit() {
}

Stage::Stage() {
	initialized = false;

	imagesAbove = imagesBelow = nullptr;
	width = height = widthAbs = heightAbs = 0;
}

/*Stage::Stage(const char* _name)
{

}*/

Stage::~Stage() {
	delete imagesAbove;
	delete imagesBelow;
}

void Stage::create(std::string _name) {
	this->name = std::move(_name);

	thumbnail = Animation("stages/" + name + "/thumbnail.gif");
}

void Stage::init() {
	initialized = true;
	//Load the stage data from the file
	parseFile("stages/" + name + "/stage.ubu");
}

void Stage::think() {
	if(!initialized) {
		init();
	}
	if(imagesAbove) {
		imagesAbove->think();
	}
	if(imagesBelow) {
		imagesBelow->think();
	}
}

void Stage::draw(bool _above) {
	if(_above) {
		graphics::setColor(255, 255, 255, 0.5f);
		if(imagesAbove) {
			imagesAbove->draw(true);
		}
	} else {
		if(imagesBelow) {
			imagesBelow->draw(true);
		}
	}
}

void Stage::reset() {
	if(imagesAbove) {
		imagesAbove->reset();
	}
	if(imagesBelow) {
		imagesBelow->reset();
	}
}

void Stage::parseFile(const std::string& szFileName) {
	Parser parser;
	std::string path = util::getPath(szFileName);
	if(!parser.open(path)) {
		die("Cannot parse file \"" + path + "\"");
	}

	//Get all the data
	while(parser.parseLine()) {
		int argc = parser.getArgC();

		//Parse it
		bool above = parser.is("IMAGE_A", 2);
		if(above || parser.is("IMAGE_B", 2)) {
			float parallax = parser.getArgFloat(2);
			int round = 0;
			float xvel = 0.0f;
			float yvel = 0.0f;
			bool wrap = false;
			if(argc > 3) {
				round = parser.getArgInt(3);
				if(argc > 4) {
					xvel = parser.getArgFloat(4);
					if(argc > 5) {
						yvel = parser.getArgFloat(5);
						if(argc > 6) {
							wrap = parser.getArgBool(6, false);
						}
					}
				}
			}

			//Add a new image
			Image imgData;
			imgData.createFromFile(getResource(parser.getArg(1), "png"));
			if(!imgData.exists()) {
				continue;
			}
			SceneImage* newImg = new SceneImage(imgData, 0, 0, parallax, RENDER_NORMAL, xvel, yvel, wrap, round);

			if(above) {
				if(imagesAbove) {
					SceneImage* img = imagesAbove;
					for(; img->next; img = img->next);
					img->next = newImg;
				} else {
					imagesAbove = newImg;
				}
			} else {
				if(imagesBelow) {
					SceneImage* img = imagesBelow;
					for(; img->next; img = img->next);
					img->next = newImg;
				} else {
					imagesBelow = newImg;
				}
			}
		} else if(parser.is("WIDTH", 1)) {
			width = parser.getArgInt(1);
		} else if(parser.is("HEIGHT", 1)) {
			height = parser.getArgInt(1);
		} else if(parser.is("WIDTH_ABS", 1)) {
			widthAbs = parser.getArgInt(1);
		} else if(parser.is("HEIGHT_ABS", 1)) {
			heightAbs = parser.getArgInt(1);
		} else if(parser.is("BGM", 1)) {
			if(argc > 2) {
				bgm.createFromFile(getResource(parser.getArg(1), EXT_MUSIC), getResource(parser.getArg(2), EXT_MUSIC));
			} else {
				bgm.createFromFile("", getResource(parser.getArg(1), EXT_MUSIC));
			}
		} else if(parser.is("BGM2", 1)) {
			if(argc > 2) {
				bgm2.createFromFile(getResource(parser.getArg(1), EXT_MUSIC), getResource(parser.getArg(2), EXT_MUSIC));
			} else {
				bgm2.createFromFile("", getResource(parser.getArg(1), EXT_MUSIC));
			}
		}
	}
}

std::string Stage::getResource(const std::string& resource, const std::string& extension) {
	if(*resource.c_str() == '*') {
		return "stages/common/" + resource.substr(1, std::string::npos) + "." + extension;
	} else {
		return "stages/" + name + "/" + resource + "." + extension;
	}
}

void Stage::bgmPlay() {
	if(bgm.exists()) {
		if(bgm2.exists()) {
			if(util::roll(2)) {
				bgm.play();
			} else {
				bgm2.play();
			}
		} else {
			bgm.play();
		}
	}
}
