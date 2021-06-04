#include <string.h>

#include "parser.h"
#include "error.h"
#include "util.h"

ParserLine::ParserLine() :
	argv()
{
	group = false;
	argc = 0;
}

ParserLine::~ParserLine() {
}

bool Parser::open(std::string szFileName) {
	lines.clear();
	util::freeLines(szLines);
	iLine = -1;

	if((szLines = util::getLinesFromFile(&nLines, szFileName))) {
		lines.resize(nLines);

		for(int i = 0; i < nLines; i++) {
			int size = strlen(szLines[i]);

			//Is this a [group]?
			if(szLines[i][0] == '[') {
				//Make sure it ends in a ']'
				if(szLines[i][size-1] != ']') {
					return false;
				}

				lines[i].group = true;

				//Parse the words
				for(int start = 1, end = 0; szLines[i][start] && lines[i].argc < ParserLine::ARGV_SIZE; start = end + 1) {
					util::nextWord(szLines[i], start, size, &start, &end);
					szLines[i][end] = 0;

					//Save the pointer to the arg list
					lines[i].argv[lines[i].argc++] = szLines[i] + start;
				}
			} else {
				//Parse the args
				for(int start = 0, end = 0; szLines[i][start] && lines[i].argc < ParserLine::ARGV_SIZE; start = end + 1) {
					//Parse the words
					util::nextWord(szLines[i], start, size, &start, &end);

					//Save the pointer to the arg list
					lines[i].argv[lines[i].argc++] = std::string(szLines[i] + start, szLines[i] + end);

					//End if we've reached a null
					if(!szLines[i][end]) {
						break;
					}
					szLines[i][end] = 0;
				}
			}
		}

		return true;
	}
	lines.clear();
	return false;
}

void Parser::reset() {
	iLine = -1;
}

bool Parser::exists() const {
	return szLines != nullptr;
}

Parser::Parser() {
	iLine = -1;
	nLines = 0;
	szLines = nullptr;
	lines.clear();
}

Parser::Parser(std::string szFileName) {
	szLines = nullptr;
	lines.clear();
	open(szFileName);
}

Parser::~Parser() {
	util::freeLines(szLines);
}

bool Parser::parseLine() {
	if(!szLines) {
		return false;
	}

	iLine++;

	if(iLine >= nLines) {
		return false;
	}

	return true;
}

bool Parser::is(std::string szTest, int argc) const {
	if(!szTest.compare(lines[iLine].argv[0])) {
		if(lines[iLine].argc - 1 < argc) {
			error::error("Not enough arguments for field \"" + szTest + "\".");
			return false;
		}
		return true;
	}
	return false;
}

bool Parser::isGroup() const {
	if(!szLines) {
		return false;
	}
	return lines[iLine].group;
}

int Parser::getArgC() const {
	return lines[iLine].argc;
}

std::string Parser::getArg(int arg) const {
	if(!szLines) {
		return "";
	}
	if(arg < 0 || arg >= lines[iLine].argc) {
		return "";
	}
	return lines[iLine].argv[arg];
}

int Parser::getArgInt(int arg) const {
	if(!szLines) {
		return 0;
	}
	if(arg < 0 || arg >= lines[iLine].argc) {
		return 0;
	}
	return std::stoi(lines[iLine].argv[arg]);
}

float Parser::getArgFloat(int arg) const {
	if(!szLines) {
		return 0.0;
	}
	if(arg < 0 || arg >= lines[iLine].argc) {
		return 0.0;
	}
	return std::stof(lines[iLine].argv[arg], nullptr);
}

bool Parser::getArgBool(int arg, bool def) const {
	if(!szLines) {
		return def;
	}
	if(arg < 0 || arg >= lines[iLine].argc) {
		return def;
	}
	return util::strtobool(lines[iLine].argv[arg], def);
}
