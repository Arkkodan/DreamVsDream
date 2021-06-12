#ifndef DVD_INPUT_H
#define DVD_INPUT_H

#ifdef SPRTOOL
#include "sprite.h"
#endif // SPRTOOL

namespace input {
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

#ifdef SPRTOOL
  extern bool selectBoxAttack;
  extern bool selectAll;
  extern sprite::HitBox *selectBox;
#endif // SPRTOOL
} // namespace input

#endif // DVD_INPUT_H
