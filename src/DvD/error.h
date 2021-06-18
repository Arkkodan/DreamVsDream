#ifndef DVD_ERROR_H
#define DVD_ERROR_H

#include <string>

namespace error {
  /// @brief Report a non-fatal warning
  void error(const std::string &sz);

  /// @brief Report a fatal error and terminate
  void die(const std::string &sz);

#ifdef DEBUG
  void debug(const std::string &sz);
#endif
} // namespace error

#endif // DVD_ERROR_H
