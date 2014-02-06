#ifndef STAGE_H_INCLUDED
#define STAGE_H_INCLUDED

#include "globals.h"

#include "menu.h"

extern int stage;
#define STAGE stages[stage]

class Stage {
public:
	std::string name;
	MenuImage* imagesAbove;
	MenuImage* imagesBelow;
	audio::Music bgm;
	audio::Music bgm2;

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
extern const char* szStages[];

#endif // STAGE_H_INCLUDED
