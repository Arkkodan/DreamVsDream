#ifndef UTIL_H_INCLUDED
#define UTIL_H_INCLUDED

#include "globals.h"

#define MIN(X,Y) ((X) < (Y) ? (X) : (Y))
#define MAX(X,Y) ((X) > (Y) ? (X) : (Y))
#undef ARRAYSIZE
#define ARRAYSIZE(x) (sizeof(x) / sizeof((x)[0]))

namespace util {
	class Vector {
	public:
		Vector();
		Vector(int x, int y);

		Vector& operator+=(const Vector& other);
		Vector& operator-=(const Vector& other);

		Vector& operator*=(const int scalar);
		Vector operator*(const int scalar);

		int x;
		int y;
	};

	class Vectorf {
	public:
		Vectorf();
		Vectorf(float x, float y);

		Vectorf& operator+=(const Vectorf& other);
		Vectorf& operator-=(const Vectorf& other);

		Vectorf& operator*=(const float scalar);
		Vectorf operator*(const float scalar);

		float x;
		float y;
	};

	//This is equivalent to fopen, but works with unicode filenames
	//on Windows.
	FILE* fopen8(std::string szFileName, const char* flags);

#ifdef _WIN32
	wchar_t* utf8to16(const char* string);
	char* utf16to8(const wchar_t* string);
#endif

	//This function determines if the character(s) at c
	//represent a newline. If it does, it returns the number
	//of characters that the newline uses.
	int isNewline(char* c, int size);

	//Same, but with whitespace. This does NOT consider newlines
	//to be whitespace!
	int isWhitespace(char* c, int size);

	//Determines if string is "true" or "false", returning def
	//if it matches neither.
	bool strtobool(const char* str, bool def);

	//This determines the start and the end position of the next word.
	//The start index is written to start, the end to end.
	void nextWord(char* c, int index, int size, int* start, int* end);

	//These functions read a utf-8 encoded file and return the lines
	//as a character array.
	char** getLinesFromFile(int* n, std::string szFileName);
	void freeLines(char** lines);

	//RNG
	int roll(int size);
	int roll(int min, int max);
	float rollf();

	//Number->string conversion
	std::string toString(int);

	//Directory listing
	std::vector<std::string> listDirectory(std::string directory, bool listFiles);
}

//Position of the camera
extern util::Vector cameraPos;

#endif // UTIL_H_INCLUDED
