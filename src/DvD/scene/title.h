#ifndef DVD_SCENE_TITLE_H
#define DVD_SCENE_TITLE_H

#include "scene_base.h"

#include "../font.h"

/// @brief Scene for the title menu
class SceneTitle : public Scene {
private:
	enum {
		TM_MAIN,
		TM_VERSUS,

		TM_MAX
	};

	enum {
		CHOICE_ARCADE,
		CHOICE_STORY,
		CHOICE_VERSUS,
		CHOICE_SURVIVAL,
		CHOICE_TRAINING,
#ifndef NO_NETWORK
		CHOICE_NETPLAY,
#endif
		CHOICE_OPTIONS,
		CHOICE_QUIT,

		CHOICE_MAX
	};

	enum {
		CHOICE_VS_PLR,
		CHOICE_VS_CPU,
		CHOICE_VS_TAG,
		CHOICE_VS_TEAM,

		CHOICE_VS_RETURN,

		CHOICE_VS_MAX
	};

	static const char* menuChoicesMain[CHOICE_MAX];
	static const char* menuChoicesVersus[CHOICE_VS_MAX];
	static const char** menuChoices[TM_MAX];
	static const int menuChoicesMax[TM_MAX];

public:
	SceneTitle();
	~SceneTitle();

	void init();

	std::string* themes;
	int nThemes;

	//Members
	int menuX, menuY;
	Font menuFont;
	int menuXOffset;

	uint8_t iR, iG, iB, aR, aG, aB;
	int aXOffset;

	int choiceTimer;
	uint8_t choiceLast;
	uint8_t choice;
	uint8_t submenu;

	//Functions
	void think();
	void reset();
	void draw();

	void parseLine(Parser& parser);
};

#endif // DVD_SCENE_TITLE_H
