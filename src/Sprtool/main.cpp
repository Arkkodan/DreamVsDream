// We have a main function
#define SDL_MAIN_HANDLED

#include <stdio.h>
#include <iostream>

#ifndef _WIN32
#include <unistd.h>
#endif

#include "../DvD/parser.h"
#include "../DvD/fighter.h"
#include "../DvD/graphics.h"
#include "../DvD/error.h"
#include "../DvD/globals.h"
#include "../DvD/error.h"

game::Fighter fighter;
int frame = 0;
int anim = 0;

namespace input {
extern bool blackBG;
}

namespace init {
	extern void init();
}

void moveFile(std::string old, std::string nw) {
#ifdef _WIN32
	WCHAR* old16 = util::getPathUtf16(old);
	WCHAR* nw16 = util::getPathUtf16(nw);
	DWORD dwAttrib = GetFileAttributesW(nw16);
	if(dwAttrib != INVALID_FILE_ATTRIBUTES && !(dwAttrib & FILE_ATTRIBUTE_DIRECTORY)) {
		DeleteFileW(nw16);
	}
	bool err = !MoveFileW(old16, nw16);
	free(nw16);
	free(old16);
#else
	unlink(nw.c_str());
	bool err = rename(old.c_str(), nw.c_str()) == -1;
#endif
	if(err) {
		error::error("Could not move file \"" + old + "\" to \"" + nw + "\"");
	}
}

namespace game {
void Fighter::create(std::string name_) {
	name = name_;

	//Palettes
	palettes = nullptr;

	//Sprites
	Parser parser("chars/" + name + "/sprites.ubu");

	//First pass
	nSprites = 0;
	while(parser.parseLine()) {
		if(parser.isGroup()) {
			nSprites++;
		}
	}
	sprites = new sprite::Sprite[nSprites];

	//Parse the damned sprites for god's sake
	//Second pass
	parser.reset();

	int hitboxCounter = 0;
	int attackCounter = 0;
	int i = -1;
	while(parser.parseLine()) {
		int argc = parser.getArgC();
		if(parser.isGroup()) {
			//There's a new sprite definition! Load that shit
			i++;
			hitboxCounter = 0;
			attackCounter = 0;

			sprites[i].name = parser.getArg(0);
			if(argc == 5) {
				sprites[i].x = parser.getArgInt(1);
				sprites[i].y = parser.getArgInt(2);

				sprites[i].hitBoxes.init(parser.getArgInt(3));
				sprites[i].aHitBoxes.init(parser.getArgInt(4));
			} else {
				sprites[i].x = 0;
				sprites[i].y = 0;

				sprites[i].hitBoxes.init(0);
				sprites[i].aHitBoxes.init(0);
			}

			//The image. Load it.
			sprites[i].img.createFromFile("chars/" + name + "/sprites/" + parser.getArg(0) + ".png");
			continue;
		}

		if(argc == 4) {
			if(hitboxCounter < sprites[i].hitBoxes.size) {
				sprites[i].hitBoxes.boxes[hitboxCounter].pos.x = parser.getArgInt(0);
				sprites[i].hitBoxes.boxes[hitboxCounter].pos.y = parser.getArgInt(1);
				sprites[i].hitBoxes.boxes[hitboxCounter].size.x = parser.getArgInt(2);
				sprites[i].hitBoxes.boxes[hitboxCounter].size.y = parser.getArgInt(3);
				hitboxCounter++;
			} else if(attackCounter < sprites[i].aHitBoxes.size) {
				sprites[i].aHitBoxes.boxes[attackCounter].pos.x = parser.getArgInt(0);
				sprites[i].aHitBoxes.boxes[attackCounter].pos.y = parser.getArgInt(1);
				sprites[i].aHitBoxes.boxes[attackCounter].size.x = parser.getArgInt(2);
				sprites[i].aHitBoxes.boxes[attackCounter].size.y = parser.getArgInt(3);
				attackCounter++;
			}
		}
	}
}

void Fighter::saveSpr() {
	//Initialize buffers
	moveFile("chars/" + name + "/sprites.ubu", "chars/" + name + "/sprites.ubu.bak");
	FILE* out = util::ufopen("chars/" + name + "/sprites.ubu", "wb");
	if(!out) {
		error::error("Could not write to file \"chars/" + name + "/sprites.ubu\"");
		return;
	}
	
	for(int i = 0; i < nSprites; i++) {
		fprintf(out, "\r\n[%s %d, %d, %d, %d]\r\n", sprites[i].name.c_str(), sprites[i].x, sprites[i].y, sprites[i].hitBoxes.size, sprites[i].aHitBoxes.size);
		for(int j = 0; j < sprites[i].hitBoxes.size; j++) {
			fprintf(out, "%d, %d, %d, %d\r\n",
					sprites[i].hitBoxes.boxes[j].pos.x,
					sprites[i].hitBoxes.boxes[j].pos.y,
					sprites[i].hitBoxes.boxes[j].size.x,
					sprites[i].hitBoxes.boxes[j].size.y);
		}
		for(int j = 0; j < sprites[i].aHitBoxes.size; j++) {
			fprintf(out, "%d, %d, %d, %d\r\n",
					sprites[i].aHitBoxes.boxes[j].pos.x,
					sprites[i].aHitBoxes.boxes[j].pos.y,
					sprites[i].aHitBoxes.boxes[j].size.x,
					sprites[i].aHitBoxes.boxes[j].size.y);
		}
	}
	fclose(out);
}
}

#ifdef _WIN32
int main(int foo, char** bar)
#else
int main(int argc, char** argv)
#endif
{
#ifdef _WIN32
	//Get argc/argv
	int argc = 0;
	WCHAR** argv = CommandLineToArgvW(GetCommandLineW(), &argc);
	if(argc < 2) {
		LocalFree(argv);
		return 1;
	}
	char* name8 = util::utf16to8(argv[1]);
	LocalFree(argv);
	std::string name = name8;
	free(name8);
#else
	if(argc < 2) {
		std::cerr << "error: no fighter specified." << std::endl;
		return 1;
	}
	std::string name = argv[1];
#endif

	init::init();

	fighter.create(name);

	for(;;) {
		os::refresh();

		//Draw a crosshair
		glBindTexture(GL_TEXTURE_2D, 0);
		if(input::blackBG) {
			glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
		} else {
			glColor4f(0.0f, 0.0f, 0.0f, 1.0f);
		}
		glBegin(GL_QUADS);
		glVertex3f(globals::WINDOW_WIDTH / 2 - 100, globals::FLIP(globals::EDIT_OFFSET), 0);
		glVertex3f(globals::WINDOW_WIDTH / 2 - 100, globals::FLIP(globals::EDIT_OFFSET)+1, 0);
		glVertex3f(globals::WINDOW_WIDTH / 2 + 99, globals::FLIP(globals::EDIT_OFFSET)+1, 0);
		glVertex3f(globals::WINDOW_WIDTH / 2 + 99, globals::FLIP(globals::EDIT_OFFSET), 0);
		glEnd();
		glBegin(GL_QUADS);
		glVertex3f(globals::WINDOW_WIDTH / 2+1, globals::FLIP(globals::EDIT_OFFSET) - 4, 0);
		glVertex3f(globals::WINDOW_WIDTH / 2-1, globals::FLIP(globals::EDIT_OFFSET) - 4, 0);
		glVertex3f(globals::WINDOW_WIDTH / 2-1, globals::FLIP(globals::EDIT_OFFSET) + 5, 0);
		glVertex3f(globals::WINDOW_WIDTH / 2+1, globals::FLIP(globals::EDIT_OFFSET) + 5, 0);
		glEnd();
		glColor4f(1.0f, 1.0f, 1.0f, 1.0f);

		fighter.sprites[frame].draw(0, globals::EDIT_OFFSET, false, 1.0f);
	}

	return 0;
}
