#ifndef STAGE_H_INCLUDED
#define STAGE_H_INCLUDED

#include "globals.h"

#include "menu.h"
#include "animation.h"

extern int stage;
#define STAGE stages[stage]

/// @brief Stage class including creation, logic, and drawing
class Stage {
public:
	std::string name;
	MenuImage* imagesAbove;
	MenuImage* imagesBelow;
	audio::Music bgm;
	audio::Music bgm2;

	Animation thumbnail;

	bool initialized;

	int width;
	int height;
	int widthAbs;
	int heightAbs;

	Stage();
	//Stage(const char* name);
	~Stage();

	void create(std::string name);

	void init();

	void think();
	void reset();
	void draw(bool above);

	void bgmPlay();

	void parseFile(const std::string& szFileName);
	std::string getResource(const std::string& szFileName, const std::string& extension);

	static void ginit();
	static void deinit();
};

extern Stage stages[];

#endif // STAGE_H_INCLUDED
