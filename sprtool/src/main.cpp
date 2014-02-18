#include <stdio.h>
#include <iostream>

#ifndef _WIN32
#include <unistd.h>
#endif

#include "../../DvD/src/parser.h"
#include "../../DvD/src/fighter.h"
#include "../../DvD/src/graphics.h"
#include "../../DvD/src/error.h"
#include "../../DvD/src/globals.h"
#include "../../DvD/src/error.h"

game::Fighter fighter;
int frame = 0;
int anim = 0;

namespace input {
extern bool blackBG;
}

void moveFile(std::string old, std::string nw) {
#ifdef _WIN32
	WCHAR* old16 = util::toFilename(old.c_str());
	WCHAR* nw16 = util::toFilename(nw.c_str());
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
		error("Could not move file \"" + old + "\" to \"" + nw + "\"");
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
	Parser parser("chars/" + name + "/sprites.ubu.bak");
	FILE* out = util::ufopen("chars/" + name + "/sprites.ubu", "wb");
	if(!out) {
		error("Could not write to file \"chars/" + name + "/sprites.ubu\"");
		return;
	}

	int index = -1;
	int hitboxCounter = 0;
	int attackCounter = 0;
	int oldHitCount = 0;
	int oldAHitCount = 0;
	while(parser.parseLine()) {
		int argc = parser.getArgC();
		if(parser.isGroup()) {
			index++;
			fprintf(out, "\r\n[%s %d, %d, %d, %d]\r\n", sprites[index].name.c_str(), sprites[index].x, sprites[index].y, sprites[index].hitBoxes.size, sprites[index].aHitBoxes.size);
			hitboxCounter = 0;
			attackCounter = 0;
			oldHitCount = 0;
			oldAHitCount = 0;
			if(argc == 5) {
				oldHitCount = parser.getArgInt(3);
				oldAHitCount = parser.getArgInt(4);
			}

			if(!oldHitCount) {
				while(hitboxCounter < sprites[index].hitBoxes.size) {
					fprintf(out, "%d, %d, %d, %d\r\n",
					        sprites[index].hitBoxes.boxes[hitboxCounter].pos.x,
					        sprites[index].hitBoxes.boxes[hitboxCounter].pos.y,
					        sprites[index].hitBoxes.boxes[hitboxCounter].size.x,
					        sprites[index].hitBoxes.boxes[hitboxCounter].size.y);
					hitboxCounter++;
				}
			}

			if(!oldAHitCount) {
				while(attackCounter < sprites[index].aHitBoxes.size) {
					fprintf(out, "%d, %d, %d, %d\r\n",
					        sprites[index].aHitBoxes.boxes[attackCounter].pos.x,
					        sprites[index].aHitBoxes.boxes[attackCounter].pos.y,
					        sprites[index].aHitBoxes.boxes[attackCounter].size.x,
					        sprites[index].aHitBoxes.boxes[attackCounter].size.y);
					attackCounter++;
				}
			}
			continue;
		}

		if(argc == 4) {
			if(hitboxCounter < sprites[index].hitBoxes.size) {
				fprintf(out, "%d, %d, %d, %d\n",
				        sprites[index].hitBoxes.boxes[hitboxCounter].pos.x,
				        sprites[index].hitBoxes.boxes[hitboxCounter].pos.y,
				        sprites[index].hitBoxes.boxes[hitboxCounter].size.x,
				        sprites[index].hitBoxes.boxes[hitboxCounter].size.y);
				hitboxCounter++;

				if(hitboxCounter == oldHitCount) {
					while(hitboxCounter < sprites[index].hitBoxes.size) {
						fprintf(out, "%d, %d, %d, %d\n",
						        sprites[index].hitBoxes.boxes[hitboxCounter].pos.x,
						        sprites[index].hitBoxes.boxes[hitboxCounter].pos.y,
						        sprites[index].hitBoxes.boxes[hitboxCounter].size.x,
						        sprites[index].hitBoxes.boxes[hitboxCounter].size.y);
						hitboxCounter++;
					}
				}
			} else if(attackCounter < sprites[index].aHitBoxes.size) {
				if(hitboxCounter < oldHitCount) {
					hitboxCounter++;
				} else {
					fprintf(out, "%d, %d, %d, %d\n",
					        sprites[index].aHitBoxes.boxes[attackCounter].pos.x,
					        sprites[index].aHitBoxes.boxes[attackCounter].pos.y,
					        sprites[index].aHitBoxes.boxes[attackCounter].size.x,
					        sprites[index].aHitBoxes.boxes[attackCounter].size.y);
					attackCounter++;

					if(attackCounter == oldAHitCount) {
						while(attackCounter < sprites[index].aHitBoxes.size) {
							fprintf(out, "%d, %d, %d, %d\n",
							        sprites[index].aHitBoxes.boxes[attackCounter].pos.x,
							        sprites[index].aHitBoxes.boxes[attackCounter].pos.y,
							        sprites[index].aHitBoxes.boxes[attackCounter].size.x,
							        sprites[index].aHitBoxes.boxes[attackCounter].size.y);
							attackCounter++;
						}
					}
				}
			}
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

	void init();
	init();

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
		glVertex3f(WINDOW_WIDTH / 2 - 100, FLIP(EDIT_OFFSET), 0);
		glVertex3f(WINDOW_WIDTH / 2 - 100, FLIP(EDIT_OFFSET)+1, 0);
		glVertex3f(WINDOW_WIDTH / 2 + 99, FLIP(EDIT_OFFSET)+1, 0);
		glVertex3f(WINDOW_WIDTH / 2 + 99, FLIP(EDIT_OFFSET), 0);
		glEnd();
		glBegin(GL_QUADS);
		glVertex3f(WINDOW_WIDTH / 2+1, FLIP(EDIT_OFFSET) - 4, 0);
		glVertex3f(WINDOW_WIDTH / 2-1, FLIP(EDIT_OFFSET) - 4, 0);
		glVertex3f(WINDOW_WIDTH / 2-1, FLIP(EDIT_OFFSET) + 5, 0);
		glVertex3f(WINDOW_WIDTH / 2+1, FLIP(EDIT_OFFSET) + 5, 0);
		glEnd();
		glColor4f(1.0f, 1.0f, 1.0f, 1.0f);

		fighter.sprites[frame].draw(0, EDIT_OFFSET, false, 1.0f);
	}

	return 0;
}
