#ifndef FONT_H_INCLUDED
#define FONT_H_INCLUDED

#include "globals.h"

#include "image.h"

class Font {
public:
	Font();
	//Font(std::string filename);
	~Font();

	void drawChar(int x, int y, char c);
	void drawChar(int x, int y, char c, ubyte_t r, ubyte_t g, ubyte_t b, float a = 1.0f);
	void drawText(int x, int y, std::string text);
	void drawText(int x, int y, std::string text, ubyte_t r, ubyte_t g, ubyte_t b, float a = 1.0f);
	int getTextWidth(std::string text);
	int getCharWidth(char c);

	void createFromFile(std::string filename);
	void destroy();
	bool exists();

	Image img;
	uint16_t pos[256];
	char width[256];

	int mono; //Size of monospace characters; 0 is variable width
	bool sensitive; //Case sensitive?

	static float xscale;
	static float yscale;
	static void setScale(float xscale, float yscale = 0.0f);
};

#endif // FONT_H_INCLUDED
