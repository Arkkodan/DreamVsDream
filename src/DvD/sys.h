#ifndef DVD_SYS_H
#define DVD_SYS_H
#ifndef COMPILER
#include <string>

#include <SDL_video.h>
#endif // COMPILER

namespace sys {
#ifdef SPRTOOL
  constexpr auto WINDOW_TITLE = "Dream vs. Dream Sprite Tool v" VERSION;
#else
  constexpr auto WINDOW_TITLE = "Dream vs. Dream v" VERSION;
#endif
  constexpr auto WINDOW_WIDTH = 640;
  constexpr auto WINDOW_HEIGHT = 480;

  constexpr auto FLIP(int x) { return WINDOW_HEIGHT - 1 - x; }

  constexpr auto FPS = 60;
  constexpr auto SPF = 1.0 / FPS;
  constexpr auto MSPF = 1000 / FPS;

  constexpr auto FLOAT_FIXED_ACCURACY = 65536;

  // SPRITE TOOL STUFF
#ifdef SPRTOOL
  constexpr auto EDIT_OFFSET = 100;
#endif

#ifndef COMPILER
  /// @brief Create GL context
  void init();
  /// @brief Delete GL context
  void deinit();

  void setTitle(const std::string &title);

  /// @brief Update one frame and draw
  void refresh();

  /// @brief Get string from clipboard
  std::string getClipboard();

  void toggleFullscreen();

  /// @brief Wait
  /// @param time ms to wait
  void sleep(unsigned int time);

  /// @brief Get elasped time
  /// @return Time in ms.
  unsigned long getTime();

  /// @brief Get elapsed time using a high resolution timer
  /// @return A counter value (call frequency to obtain time)
  uint64_t getHiResTime();
  uint64_t getHiResFrequency();

  unsigned int getFrame();
  SDL_Window *getWindow();
#endif // COMPILER
} // namespace sys

#endif // DVD_SYS_H
