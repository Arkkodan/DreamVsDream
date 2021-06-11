#ifndef DVD_SCENE_TITLE_H
#define DVD_SCENE_TITLE_H

#include "scene_base.h"

#include "../font.h"

namespace scene {

	/// @brief Scene for the title menu
	class Title : public Scene {
	public:
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

	private:
		static std::vector<std::string> menuChoicesMain;
		static std::vector<std::string> menuChoicesVersus;
		static std::array<std::vector<std::string>*, TM_MAX> menuChoices;
		static const std::array<int, TM_MAX> menuChoicesMax;

	public:
		Title();
		~Title();

		void init() override final;

		std::vector<std::string> themes;
		int nThemes;

		//Members
		int menuX, menuY;
		Font* menuFont;
		int menuXOffset;

		uint8_t iR, iG, iB, aR, aG, aB;
		int aXOffset;

		int choiceTimer;
		uint8_t choiceLast;
		uint8_t choice;
		uint8_t submenu;

		//Functions
		void think() override final;
		void reset() override final;
		void draw() const override final;

		void parseLine(Parser& parser) override final;
		void parseJSON(const nlohmann::ordered_json& j_obj) override final;
	};
}

#endif // DVD_SCENE_TITLE_H
