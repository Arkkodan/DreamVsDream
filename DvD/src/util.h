#ifndef UTIL_H_INCLUDED
#define UTIL_H_INCLUDED

#include <string.h>
#include <string>

#include "globals.h"

#define MIN(X,Y) ((X) < (Y) ? (X) : (Y))
#define MAX(X,Y) ((X) > (Y) ? (X) : (Y))
#define ARRAYSIZE(x) (sizeof(x) / sizeof((x)[0]))

FILE*
fopen8(std::string szFileName, const char* flags);

bool strtobool(const char* str, bool def);

#ifdef _WIN32
wchar_t* utf8to16(const char* string);
char* utf16to8(const wchar_t* string);
#endif

class Vector
{
public:
    Vector();
    Vector(int x, int y);

    int x;
    int y;
};

class Vectorf
{
public:
    Vectorf();
    Vectorf(float x, float y);

    Vectorf& operator+=(const Vectorf& other);
    Vectorf& operator-=(const Vectorf& other);
    Vectorf& operator*=(const float scalar);

    Vectorf operator*(const float scalar);

    //Vectorf operator+(const TRectangle& x, const TRectangle& y);

    float x;
    float y;
};

//This function determines if the character(s) at c
//represent a newline. If it does, it returns the number
//of characters that the newline uses.
int isNewline(char* c, int size);

//Same, but with whitespace. This does NOT consider newlines
//to be whitespace!
int isWhitespace(char* c, int size);
void nextWord(char* c, int index, int size, int* start, int* end);

char** getLinesFromFile(int* n, std::string szFileName);
void freeLines(char** lines);

extern Vector cameraPos;

//RNG
int roll(int size);
int roll(int min, int max);
float rollf();

#endif // UTIL_H_INCLUDED
