#ifndef PARSER_H_INCLUDED
#define PARSER_H_INCLUDED

#include "globals.h"

#define ARGV_SIZE 32

#define EXT_SCRIPT "ubu"

#define EXT_IMAGE "png"
#define EXT_SOUND "wav"
#define EXT_MUSIC "ogg"
#define EXT_FONT "rf"
#define EXT_VIDEO "avi"
#define EXT_TEXT "txt"

/// @brief Data structure to hold information about a parsed line from Parser
class ParserLine {
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
