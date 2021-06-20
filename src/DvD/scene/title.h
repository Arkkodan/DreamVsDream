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
      TITLE_MAIN_SINGLEPLAYER,
      TITLE_MAIN_MULTIPLAYER,
      TITLE_MAIN_OPTIONS,
      TITLE_MAIN_QUIT,

      TITLE_MAIN_MAX
    };
    enum {
      TITLE_SINGLEPLAYER_ARCADE,
      TITLE_SINGLEPLAYER_STORY,
      TITLE_SINGLEPLAYER_VS_CPU,
      TITLE_SINGLEPLAYER_SURVIVAL,
      TITLE_SINGLEPLAYER_TRAINING,

      TITLE_SINGLEPLAYER_MAX
    };
    enum {
      TITLE_MULTIPLAYER_VS_PLAYER,
      TITLE_MULTIPLAYER_TAG,
      TITLE_MULTIPLAYER_TEAM,
      TITLE_MULTIPLAYER_NETPLAY,

      TITLE_MULTIPLAYER_MAX
    };

  private:
    static const std::array<std::string, TITLE_MAIN_MAX> submenuMainStrings;
    static const std::array<std::string, TITLE_SINGLEPLAYER_MAX>
        submenuSPStrings;
    static const std::array<std::string, TITLE_MULTIPLAYER_MAX>
        submenuMPStrings;

  private:
    std::vector<std::string> themes;
    int nThemes;

    // Members
    int menuX, menuY;
    Font *menuFont;
    int menuXOffset;

    menu::MainSubmenuB submenuMain;
    menu::Submenu submenuSP;
    menu::Submenu submenuMP;

    uint8_t iR, iG, iB, aR, aG, aB;
    int aXOffset;
  };
} // namespace scene

#endif // DVD_SCENE_TITLE_H
