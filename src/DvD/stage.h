#ifndef STAGE_H_INCLUDED
#define STAGE_H_INCLUDED

#include "scene/scene.h"
#include "animation.h"

#define STAGE Stage::stages[Stage::stage]

/// @brief Stage class including creation, logic, and drawing
class Stage {
public:
	static int stage;
	static Stage stages[];

public:
	std::string name;
	SceneImage* imagesAbove;
	SceneImage* imagesBelow;
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

#endif // STAGE_H_INCLUDED
