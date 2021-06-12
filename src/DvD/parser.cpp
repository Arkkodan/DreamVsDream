#include "parser.h"

#include "error.h"
#include "../fileIO/text.h"
#include "../util/fileIO.h"

#include <cstring>

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
	iLine = -1;

	szLines = fileIO::readTextAsLines(szFileName);
	nLines = szLines.size();

	if (!szLines.empty()) {
		lines.resize(nLines);

		for(int i = 0; i < nLines; i++) {
			int size = szLines[i].size();

			//Is this a [group]?
			if(szLines[i].front() == '[') {
				//Make sure it ends in a ']'
				if(szLines[i].back() != ']') {
					return false;
				}

				lines[i].group = true;

				//Parse the words
				for(int start = 1, end = 0; start < size && lines[i].argc < ParserLine::ARGV_SIZE; start = end + 1) {
					// TODO: Remove const_cast
					util::nextWord(const_cast<char*>(szLines[i].c_str()), start, size, &start, &end);

					//Save the pointer to the arg list
					lines[i].argv[lines[i].argc++] = szLines[i].substr(start, end - start);
				}
			} else {
				//Parse the args
				for(int start = 0, end = 0; start < size && lines[i].argc < ParserLine::ARGV_SIZE; start = end + 1) {
					//Parse the words
					// TODO: Remove const_cast
					util::nextWord(const_cast<char*>(szLines[i].c_str()), start, size, &start, &end);

					//Save the pointer to the arg list
					lines[i].argv[lines[i].argc++] = szLines[i].substr(start, end - start);

					//End if we've reached a null
					if(end > szLines[i].size()) {
						break;
					}
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
	return !szLines.empty();
}

Parser::Parser() {
	iLine = -1;
	nLines = 0;
	szLines.clear();
	lines.clear();
}

Parser::Parser(std::string szFileName) {
	szLines.clear();
	lines.clear();
	open(szFileName);
}

Parser::~Parser() {}

bool Parser::parseLine() {
	if (szLines.empty()) {
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
	if (szLines.empty()) {
		return false;
	}
	return lines[iLine].group;
}

int Parser::getArgC() const {
	return lines[iLine].argc;
}

std::string Parser::getArg(int arg) const {
	if (szLines.empty()) {
		return "";
	}
	if(arg < 0 || arg >= lines[iLine].argc) {
		return "";
	}
	return lines[iLine].argv[arg];
}

int Parser::getArgInt(int arg) const {
	if (szLines.empty()) {
		return 0;
	}
	if(arg < 0 || arg >= lines[iLine].argc) {
		return 0;
	}
	return std::stoi(lines[iLine].argv[arg]);
}

float Parser::getArgFloat(int arg) const {
	if (szLines.empty()) {
		return 0.0;
	}
	if(arg < 0 || arg >= lines[iLine].argc) {
		return 0.0;
	}
	return std::stof(lines[iLine].argv[arg], nullptr);
}

bool Parser::getArgBool(int arg, bool def) const {
	if (szLines.empty()) {
		return def;
	}
	if(arg < 0 || arg >= lines[iLine].argc) {
		return def;
	}
	return util::strtobool(lines[iLine].argv[arg], def);
}
