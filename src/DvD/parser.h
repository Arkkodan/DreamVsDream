#ifndef DVD_PARSER_H
#define DVD_PARSER_H

#include <array>
#include <string>
#include <vector>

/// @brief Data structure to hold information about a parsed line from Parser
struct ParserLine {
  static constexpr auto ARGV_SIZE = 32;

  bool group;
  int argc;
  std::array<std::string, ARGV_SIZE> argv;
};

/// @brief Parser for a text file
/// @details Designed for ubu scripts
class Parser {
public:
  static constexpr auto EXT_SCRIPT = "ubu";

  static constexpr auto EXT_IMAGE = "png";
  static constexpr auto EXT_SOUND = "wav";
  static constexpr auto EXT_MUSIC = "ogg";
  static constexpr auto EXT_FONT = "rf";
  static constexpr auto EXT_VIDEO = "avi";
  static constexpr auto EXT_TEXT = "txt";

public:
  bool open(std::string szFileName);
  void reset();
  bool exists() const;

  Parser();
  Parser(std::string szFileName);
  ~Parser();

  bool parseLine();

  bool isGroup() const;
  int getArgC() const;

  std::string getArg(int arg) const;
  int getArgInt(int arg) const;
  float getArgFloat(int arg) const;
  bool getArgBool(int arg, bool def) const;

  bool is(std::string szTest, int argc) const;

private:
  int iLine;
  int nLines;
  std::vector<std::string> szLines;
  std::vector<ParserLine> lines;
};

#endif // DVD_PARSER_H
