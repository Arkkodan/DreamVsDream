#ifndef UTIL_FILEIO_H
#define UTIL_FILEIO_H

#include <string>
#include <vector>

namespace util {

  /// @brief Unicode-ambivalent fopen
  /// @details Equivalent to fopen but works with unicode filenames on Windows
  /// @param szFileName Filename to open
  /// @param flags flags parameter to fopen
  /// @return FILE pointer to the file.
  FILE *ufopen(const std::string &szFileName, const char *flags);

#ifdef _WIN32
  /// @brief Convert UTF-8 to UTF-16
  wchar_t *utf8to16(const char *string);

  /// @brief Convert UTF-16 to UTF-8
  char *utf16to8(const wchar_t *string);

  /// @brief Prepend necessary information for Windows
  wchar_t *getPathUtf16(const std::string &filename);
#endif

  /// @brief Retrieve the correct filepath from a filename
  /// @detail For DvD, prepend the data directory
  std::string getPath(const std::string &filename);

  /// @brief Check if the file exists
  bool fileExists(const std::string &filename);

  /// @brief Determine if the character(s) at a pointer represents a newline
  /// @param c Pointer to a char
  /// @param Max number of chars to check
  /// @return Number of characters that this newline uses.
  /// If c does not represent a newline, return 0.
  int isNewline(char *c, int size);

  /// @brief Determine if the character(s) at a pointer represents whitespace
  /// @details Newlines are NOT considered whitespace.
  /// @param c Pointer to a char
  /// @param Max number of chars to check.
  /// @return Number of characters that this whitespace uses.
  int isWhitespace(char *c, int size);

  /// @brief Read a string for "true" or "false"
  /// @details This string is case insensitive.
  /// @param str String
  /// @param def Default bool value
  /// @return Boolean value that str has.
  /// If str is neither "true" or "false", return def.
  bool strtobool(const std::string &str, bool def);

  /// @brief Determine the start and end positions of the next word
  /// @details Words are delimited by commas and whitespaces.
  /// @param[in] c String
  /// @param[in] index Starting index to look
  /// @param[in] size Ending index to look
  /// @param[out] start Start index of the next word
  /// @param[out] end End index of the next word
  // The start index is written to start, the end to end.
  void nextWord(char *c, int index, int size, int *start, int *end);

  // These functions read a utf-8 encoded file and return the lines
  // as a character array.
  char **getLinesFromFile(int *n, const std::string &szFileName);
  void freeLines(char **lines);

  /// @brief Convert int to std::string
  std::string toString(int);

  /// @brief Retrieve a list of directories OR files
  /// @param directory The directory to look in
  /// @param listFiles Look for files if true, look for directories if false
  /// @return std::vector of directories or files specified by listFiles
  std::vector<std::string> listDirectory(const std::string &directory,
                                         bool listFiles);
} // namespace util

#endif // UTIL_FILEIO_H
