#ifndef DVD_SCENE_CONTROLS_H
#define DVD_SCENE_CONTROLS_H

#include "scene_base.h"

#include "../font.h"
#include "../menu/submenu.h"

#include <string>

#include <SDL_keycode.h>

namespace scene {

  class Controls : public Scene {
  public:
    Controls();
    ~Controls();

    void init() override final;

    void think() override final;
    void reset() override final;
    void draw() const override final;

    void parseJSON(const nlohmann::ordered_json &j_obj) override final;

    void applyInput(int input);

  private:
    void writeControls();

  public:
    enum {
      CONTROL_INPUT_LEFT,
      CONTROL_INPUT_RIGHT,
      CONTROL_INPUT_UP,
      CONTROL_INPUT_DOWN,
      CONTROL_INPUT_SELECT,
      CONTROL_INPUT_EXIT,

      CONTROL_INPUT_MAX
    };

  private:
    static constexpr auto INPUT_LEFT = 1 << CONTROL_INPUT_LEFT;
    static constexpr auto INPUT_RIGHT = 1 << CONTROL_INPUT_RIGHT;
    static constexpr auto INPUT_UP = 1 << CONTROL_INPUT_UP;
    static constexpr auto INPUT_DOWN = 1 << CONTROL_INPUT_DOWN;
    static constexpr auto INPUT_A = 1 << CONTROL_INPUT_SELECT;
    static constexpr auto INPUT_B = 1 << CONTROL_INPUT_EXIT;
    static_assert(INPUT_A == game::INPUT_A,
                  "CONTROL_INPUT_SELECT does not correspond to game::INPUT_A");
    static_assert(INPUT_B == game::INPUT_B,
                  "CONTROL_INPUT_EXIT does not correspond to game::INPUT_B");

  private:
    Font *font;

    int input;

    menu::MainSubmenuB submenu;

    bool waitKey;
    int changePlayer;
    int changeKeyEnum;
  };
} // namespace scene

#endif // DVD_SCENE_CONTROLS_H
