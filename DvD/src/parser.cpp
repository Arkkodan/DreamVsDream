#include <string.h>

#include "parser.h"
#include "error.h"
#include "util.h"

ParserLine::ParserLine()
{
	group = false;
	argc = 0;
}

ParserLine::~ParserLine()
{
}

bool Parser::open(std::string szFileName)
{
	delete [] lines;
	freeLines(szLines);
	iLine = -1;
	
	if((szLines = getLinesFromFile(&nLines, szFileName)))
	{
		lines = new ParserLine[nLines];
		
		for(int i = 0; i < nLines; i++)
		{
			int size = strlen(szLines[i]);

			//Is this a [group]?
			if(szLines[i][0] == '[')
			{
				//Make sure it ends in a ']'
				if(szLines[i][size-1] != ']')
					return false;

				lines[i].group = true;

				//Parse the words
				for(int start = 1, end = 0; szLines[i][start] && lines[i].argc < ARGV_SIZE; start = end + 1)
				{
					nextWord(szLines[i], start, size, &start, &end);
					szLines[i][end] = 0;
				
					//Save the pointer to the arg list
					lines[i].argv[lines[i].argc++] = szLines[i] + start;
				}
			}
			else
			{
				//Parse the args
				for(int start = 0, end = 0; szLines[i][start] && lines[i].argc < ARGV_SIZE; start = end + 1)
				{
					//Parse the words
					nextWord(szLines[i], start, size, &start, &end);
					
					//Save the pointer to the arg list
					lines[i].argv[lines[i].argc++] = szLines[i] + start;

					//End if we've reached a null
					if(!szLines[i][end])
						break;
					szLines[i][end] = 0;
				}
			}
		}
		
		return true;
	}
	lines = NULL;
	return false;
}

void Parser::reset()
{
    iLine = -1;
}

bool Parser::exists()
{
    return szLines != NULL;
}

Parser::Parser()
{
	iLine = -1;
	nLines = 0;
    szLines = NULL;
	lines = NULL;
}

Parser::Parser(std::string szFileName)
{
    szLines = NULL;
	lines = NULL;
    open(szFileName);
}

Parser::~Parser()
{
	delete [] lines;
    freeLines(szLines);
}

bool Parser::parseLine()
{
    if(!szLines)
		return false;
	
	iLine++;
	
	if(iLine >= nLines)
		return false;

    return true;
}

bool Parser::is(std::string szTest, int argc)
{
    if(!szTest.compare(lines[iLine].argv[0]))
    {
        if(lines[iLine].argc - 1 < argc)
        {
            error("Not enough arguments for field \"" + szTest + "\".");
            return false;
        }
        return true;
    }
    return false;
}

bool Parser::isGroup()
{
	if(!szLines)
		return false;
	return lines[iLine].group;
}

int Parser::getArgC()
{
	return lines[iLine].argc;
}

const char* Parser::getArg(int arg)
{
	if(!szLines)
		return NULL;
	if(arg < 0 || arg >= lines[iLine].argc)
		return NULL;
	return lines[iLine].argv[arg];
}

int Parser::getArgInt(int arg)
{
	if(!szLines)
		return 0;
	if(arg < 0 || arg >= lines[iLine].argc)
		return 0;
	return atoi(lines[iLine].argv[arg]);
}

float Parser::getArgFloat(int arg)
{
	if(!szLines)
		return 0.0;
	if(arg < 0 || arg >= lines[iLine].argc)
		return 0.0;
	return strtof(lines[iLine].argv[arg], NULL);
}

bool Parser::getArgBool(int arg, bool def)
{
	if(!szLines)
		return def;
	if(arg < 0 || arg >= lines[iLine].argc)
		return def;
	return strtobool(lines[iLine].argv[arg], def);
}
