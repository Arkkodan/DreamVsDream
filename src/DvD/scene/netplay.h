#ifndef DVD_SCENE_NETPLAY_H
#define DVD_SCENE_NETPLAY_H

#include "scene_base.h"

#include "../font.h"
#include "../sys.h"

#include <array>

namespace scene {

	/// @brief Scene for navigating netplay and connections
	class Netplay : public Scene {
	private:
		static constexpr auto NET_FLASH_TIME = sys::FPS / 2;
		static constexpr auto NET_FLASH_HOLD_TIME = sys::FPS / 2;
		static constexpr auto NET_FADE_TIME = sys::FPS;
		static constexpr auto NET_SCALE = 2;
		static constexpr auto NET_BAR_SIZE = 120;

	public:
		Netplay();
		~Netplay();

		int choice;
		int mode;
		int digit;
		bool waiting;

		std::array<char, 5> portStr;
		uint16_t port;
		std::array<char, 12> ipStr;
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

		void think() override final;
		void draw() const override final;
		void reset() override final;
		void parseLine(Parser& parser) override final;
	};
}

#endif // DVD_SCENE_NETPLAY_H
