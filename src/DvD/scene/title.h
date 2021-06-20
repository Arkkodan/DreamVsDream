#ifndef DVD_SCENE_TITLE_H
#define DVD_SCENE_TITLE_H

#include "scene_base.h"

#include "../font.h"
#include "../menu/submenu.h"

#include <array>

namespace scene {

  /// @brief Scene for the title menu
  class Title : public Scene {
  public:
    Title();
    ~Title();

    void init() override final;

    // Functions
    void think() override final;
    void reset() override final;
    void draw() const override final;

    void parseLine(Parser &parser) override final;
    void parseJSON(const nlohmann::ordered_json &j_obj) override final;

  private:
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
    static std::array<std::string, CHOICE_MAX> menuChoicesMain;
    static std::array<std::string, CHOICE_VS_MAX> menuChoicesVersus;

  private:
    std::vector<std::string> themes;
    int nThemes;

    // Members
    int menuX, menuY;
    Font *menuFont;
    int menuXOffset;

    menu::MainSubmenuB submenuMain;
    menu::Submenu submenuVersus;

    uint8_t iR, iG, iB, aR, aG, aB;
    int aXOffset;
  };
} // namespace scene

#endif // DVD_SCENE_TITLE_H
