#ifndef DVD_SCENE_OPTIONS_H
#define DVD_SCENE_OPTIONS_H

#include "scene_base.h"

#include "../font.h"

/// @brief Scene for modifying options
class SceneOptions : public Scene {
private:
	enum {
		OPTION_DIFFICULTY,
		OPTION_WINS,
		OPTION_TIME,
		OPTION_SFX_VOLUME,
		OPTION_MUS_VOLUME,
		OPTION_VOICE_VOLUME,
		OPTION_EPILEPSY,
		OPTION_CREDITS,

		OPTION_MAX,
	};

public:
	static int optionDifficulty;
	static int optionWins;
	static int optionTime;
	static int optionSfxVolume;
	static int optionMusVolume;
	static int optionVoiceVolume;
	static bool optionEpilepsy;

public:
	SceneOptions();
	~SceneOptions();

	void init();

	//Members
	Font menuFont;

	uint8_t cursor;
	uint8_t cursorLast;
	int cursorTimer;

	int madoPos;
	int madoDir;
	int madoFrame;

	Image madoImg;
	audio::Sound madoSfxStep;
	audio::Sound madoSfxPinch;

	audio::Voice dame;
	audio::Voice muri;

	int madoWakeTimer;

	uint8_t iR, iG, iB, aR, aG, aB;
	int aXOffset;

	std::string* themes;
	int nThemes;

	//Functions
	void think();
	void reset();
	void draw();

	void parseLine(Parser& parser);
};

#endif // DVD_SCENE_OPTIONS_H
