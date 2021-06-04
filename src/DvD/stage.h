#ifndef STAGE_H_INCLUDED
#define STAGE_H_INCLUDED

#include "scene/scene.h"
#include "animation.h"

#include <array>
#include <list>

#define STAGE Stage::stages[Stage::stage]

/// @brief Stage class including creation, logic, and drawing
class Stage {
public:
	static int stage;
	static constexpr auto STAGES_MAX = 20;
	static std::array<Stage, STAGES_MAX> stages;

public:
	std::string name;
	std::list<SceneImage> imagesAbove;
	std::list<SceneImage> imagesBelow;
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
	void draw(bool above) const;

	void bgmPlay();

	void parseFile(const std::string& szFileName);
	std::string getResource(const std::string& szFileName, const std::string& extension) const;

	static void ginit();
	static void deinit();
};

#endif // STAGE_H_INCLUDED
