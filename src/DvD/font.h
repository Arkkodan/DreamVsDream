#ifndef FONT_H_INCLUDED
#define FONT_H_INCLUDED

#include "image.h"

#include <array>

/// @brief Font class for rendering
/// @details Uses Dream vs. Dream's font format
class Font {
public:
	Font();
	//Font(std::string filename);
	~Font();

	void drawChar(int x, int y, char c) const;
	void drawChar(int x, int y, char c, uint8_t r, uint8_t g, uint8_t b, float a = 1.0f) const;
	void drawText(int x, int y, std::string text) const;
	void drawText(int x, int y, std::string text, uint8_t r, uint8_t g, uint8_t b, float a = 1.0f) const;
	int getTextWidth(std::string text) const;
	int getCharWidth(char c) const;

	void createFromFile(std::string filename);
	bool exists() const;

	Image img;
	std::array<uint16_t, 256> pos;
	std::array<char, 256> width;

	int mono; //Size of monospace characters; 0 is variable width
	bool sensitive; //Case sensitive?

	static float xscale;
	static float yscale;
	static void setScale(float xscale, float yscale = 0.0f);
};

#endif // FONT_H_INCLUDED
