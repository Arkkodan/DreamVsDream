#ifndef PARSER_H_INCLUDED
#define PARSER_H_INCLUDED

#include "globals.h"

/// @brief Data structure to hold information about a parsed line from Parser
class ParserLine {
public:
	static constexpr auto ARGV_SIZE = 32;

public:
	ParserLine();
	~ParserLine();

	bool group;
	int argc;
	char* argv[ARGV_SIZE];
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
	bool exists();

	Parser();
	Parser(std::string szFileName);
	~Parser();

	bool parseLine();

	bool isGroup();
	int getArgC();

	const char* getArg(int arg);
	int getArgInt(int arg);
	float getArgFloat(int arg);
	bool getArgBool(int arg, bool def);

	bool is(std::string szTest, int argc);

protected:
	int iLine;
	int nLines;
	char** szLines;
	ParserLine* lines;
};

#endif // PARSER_H_INCLUDED
