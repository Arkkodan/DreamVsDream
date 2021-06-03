#ifndef DVD_SCENE_NETPLAY_H
#define DVD_SCENE_NETPLAY_H

#include "scene_base.h"

#include "../font.h"
#include "../sys.h"

/// @brief Scene for navigating netplay and connections
class SceneNetplay : public Scene {
private:
	static constexpr auto NET_FLASH_TIME = sys::FPS / 2;
	static constexpr auto NET_FLASH_HOLD_TIME = sys::FPS / 2;
	static constexpr auto NET_FADE_TIME = sys::FPS;
	static constexpr auto NET_SCALE = 2;
	static constexpr auto NET_BAR_SIZE = 120;

public:
	SceneNetplay();
	~SceneNetplay();

	int choice;
	int mode;
	int digit;
	bool waiting;

	char portStr[5];
	uint16_t port;
	char ipStr[12];
	uint32_t ip;

	int flashTimer;
	int flashDir;
	int drawShake;
	int barPos;

	Image imgScanlines;
	Image imgStatic;
	Image imgLogo;

	Image imgCursor;
	Font menuFont;

	audio::Sound sndOn;
	audio::Sound sndOff;

	audio::Sound sndConStart;
	audio::Sound sndConSuccess;

	audio::Music bgmWait;

	//void init();

	void updatePort(bool toint);
	void updateIp(bool toint);

	void think();
	void draw();
	void reset();
	void parseLine(Parser& parser);
};

#endif // DVD_SCENE_NETPLAY_H
