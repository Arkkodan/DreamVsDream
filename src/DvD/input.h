#ifndef DVD_INPUT_H
#define DVD_INPUT_H

#ifdef SPRTOOL
#include "sprite.h"
#endif // SPRTOOL

#include <array>
#include <string>

namespace input {
  constexpr auto KEY_CFG_FILE = "keys.cfg";

  enum {
    KEY_INPUT_LEFT,
    KEY_INPUT_RIGHT,
    KEY_INPUT_UP,
    KEY_INPUT_DOWN,
    KEY_INPUT_A,
    KEY_INPUT_B,
    KEY_INPUT_C,

    KEY_INPUT_MAX
  };

  const std::array<std::string, KEY_INPUT_MAX> KEY_LABELS = {
      "Left", "Right", "Up", "Down", "A", "B", "C",
  };

  /// @brief Initialize inputs based on a config file or defaults
  void init();

  /// @brief Apply and handle player inputs
  void refresh();

  /// @brief Action when mouse moves
  void mouseMove(int x, int y);
  /// @brief Action when mouse is pressed or released
  void mousePress(int key, bool press);
  /// @brief Action when key is pressed or released
  /// @details Some inputs are hardcoded to do certain things while some
  /// others get propagated to player inputs.
  void keyPress(int key, bool press);

  std::array<int, 2 * KEY_INPUT_MAX> &getrKeyConfig();

#ifdef SPRTOOL
  bool isBlackBG();
  sprite::HitBox *getSelectBox();
  bool isSelectBoxAttack();
  bool isSelectAll();
#endif
} // namespace input

#endif // DVD_INPUT_H
