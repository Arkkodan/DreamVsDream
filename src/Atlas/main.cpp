#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <string>
#include <iostream>

#include "../DvD/file.h"
#include "../DvD/util.h"
#include "image.h"

#define TEXTURE_SIZE 2048
#define TEXTURE_SIZE_SQ (TEXTURE_SIZE*TEXTURE_SIZE)

//Some globals
static uint16_t c_images;
static struct Image* images;
static struct Vector* xy_images;
static struct Image atlas;
static int i_atlas = 0;

inline void imgcpy(struct Image* image, int x, int y) {
	for(int j = 0; j < image->h; j++) {
		for(int i = 0; i < image->w; i++) {
            atlas.data[TEXTURE_SIZE_SQ * i_atlas + TEXTURE_SIZE * (y + j) + (x + i)] = image->data[image->w * j + i];
		}
	}
}

void populate(int x, int y, int w, int h) {
	int b_fit[256];

	int oy = 0;
	for(;;) {
		int ox = 0;

		int c_fit = 0;
		int i = 0;
		for(; i < c_images; i++) {
			//Skip over used images
			if(!images[i].data) {
				continue;
			}

			//See if it fits
			if(images[i].w <= w - ox && images[i].h <= h - oy) {
				//Add it
				images[i].x = x + ox;
				images[i].y = y + oy;
				images[i].atlas = i_atlas;
				imgcpy(images + i, x + ox, y + oy);

				//Increase X offset by the width of the image
				ox += images[i].w;

				//Delete the image and increment counter
				free(images[i].data);
				images[i].data = nullptr;

				//Add ourselves to the fit buffer
				b_fit[c_fit++] = i;
			}
		}

		//If no sprites could fit, we're done here
		if(!c_fit) {
			return;
		}

		//Now populate the area under each sprite
		for(i = 1; i < c_fit; i++) { //Skip over first; it'll never have room under it
			populate(images[b_fit[i]].x, y + oy + images[b_fit[i]].h, images[b_fit[i]].w, images[b_fit[0]].h - images[b_fit[i]].h);
		}

		//Increase height
		oy += images[b_fit[0]].h;

		//If we overstep, we're done
		if(oy >= h) {
			return;
		}
	}
}

int compare(const void* a, const void* b) {
	struct Image* img_a = (struct Image*)a;
	struct Image* img_b = (struct Image*)b;
	if(img_a->h > img_b->h) {
		return -1;
	}
	if(img_a->h < img_b->h) {
		return 1;
	}
	if(img_a->w > img_b->w) {
		return -1;
	}
	if(img_a->w < img_b->w) {
		return 1;
	}
	return 0;
}

std::string fstr(std::string sz_) {
	const char* csz = sz_.c_str();
	int len = sz_.length();
	int dot = 0;
	for(int i = len - 1; i >= -1; i--) {
		if(!dot && csz[i] == '.') {
			dot = i;
		} else if(csz[i] == '/' || csz[i] == '\\' || i == -1) {
			if(dot) {
				return sz_.substr(i + 1, dot - i - 1);
			}
			return "";
		}
	}
	return "";
}

int loadPalette(std::string szPalette, unsigned char* palette) {
	//See if we're trying to load an act or a PNG
	std::string ext = szPalette.substr(szPalette.find_last_of(".") + 1);
	std::string path = util::getPath(szPalette);

	if(!ext.compare("act")) {
		FILE* f = util::ufopen(path, "rb");
		if(!f) {
			std::cerr << "error: cannot open palette file." << std::endl;
			return 1;
		}
		if(fread(palette, 256 * 3, 1, f) != 1) {
			std::cerr << "error: problem reading from palette file." << std::endl;
			fclose(f);
			return 1;
		}
		fclose(f);
	} else if(!ext.compare("png")) {
		if(imageReadPalette(path, palette)) {
			std::cerr << "error: problem reading from palette file." << std::endl;
			return 1;
		}
	} else {
		std::cerr << "error: unrecognized palette file: " << ext << "." << std::endl;
		return 1;
	}
	return 0;
}

void usage() {
	std::cerr << "usage: atlas [options] <images>\n"\
	          "\t-o\tOutput filename prefix\n"\
	          "\t-p\tPath to palette for indexed PNGs\n"\
	          "\t-q\tPath to palette to concatonate for indexed PNGs"
	          << std::endl;
}

enum {
	ARG_NONE,
	ARG_OUTPUT,
	ARG_PALETTE,
	ARG_PALETTE2,
};

#ifdef _WIN32
int _main(int argc, char** argv);
int main() {
	//Get utf-8 argc/argv
	int argc;
	wchar_t** argv16 = CommandLineToArgvW(GetCommandLineW(), &argc);
	char** argv = (char**)malloc(argc * sizeof(char*));
	for(int i = 0; i < argc; i++) {
		argv[i] = util::utf16to8(argv16[i]);
	}
	LocalFree(argv16);

	//Enter the real main function
	int result = _main(argc, argv);

	//Clean up utf8 args
	for(int i = 0; i < argc; i++) {
		free(argv[i]);
	}
	free(argv);

	return result;
}

int _main(int argc, char** argv)
#else
int main(int argc, char** argv)
#endif
{
	int code = 0;

	std::string sz_output = "";
	std::string sz_palette = "";
	std::string sz_palette2 = "";

	//Parse arguments
	int done = 0;
	int arg_type = 0;
	int arg = 1;
	int i;
	for(; arg < argc; arg++) {
		switch(arg_type) {
		case ARG_OUTPUT:
			sz_output = argv[arg];
			arg_type = ARG_NONE;
			break;
		case ARG_PALETTE:
			sz_palette = argv[arg];
			arg_type = ARG_NONE;
			break;
		case ARG_PALETTE2:
			sz_palette2 = argv[arg];
			arg_type = ARG_NONE;
			break;
		case ARG_NONE:
		default:
			if(*argv[arg] == '-') {
				//Parse the letters
				for(i = 1; argv[arg][i]; i++) {
					switch(argv[arg][i]) {
					//Arguments which take arguments
					case 'o':
						arg_type = ARG_OUTPUT;
						break;

					case 'p':
						arg_type = ARG_PALETTE;
						break;

					case 'q':
						arg_type = ARG_PALETTE2;
						break;
					}
				}
			} else {
				//We're done
				done = 1;
			}
			break;
		}
		if(done) {
			break;
		}
	}

	if(!sz_output.length()) {
		std::cerr << "error: no output specified.\n";
		usage();
		return 1;
	}

	//Create an array of images and sprites
	c_images = argc - arg;
	if(!c_images) {
		std::cerr << "error: no files specified.\n";
		usage();
		return 1;
	}
	images = new Image[c_images];

	//Load the images
	for(i = 0; i < c_images; i++) {
		images[i].name = argv[i + arg];
		imageRead(argv[i + arg], images + i);
	}

	//Load up the palette
	unsigned char palette[256 * 3];
	unsigned char palette2[256 * 3];
    if(!sz_palette.empty()) {
        if(loadPalette(sz_palette, palette)) {
            code = 1;
            goto end;
        }

        if(sz_palette2.length()) {
            if(loadPalette(sz_palette2, palette2)) {
                code = 1;
                goto end;
            }

            //Concatonate the palettes

            //Find the first null triplet
            for(int i = 255; i >= 0; i--) {
                if(palette[i*3+0] || palette[i*3+1] || palette[i*3+2]) {
                    memcpy(palette + (i+1) * 3, palette2 + (i+1) * 3, (256 - (i+1)) * 3);
                    break;
                }
            }
        }
    }



	//Create an atlas image
	atlas.w = TEXTURE_SIZE;
	atlas.h = TEXTURE_SIZE;
	atlas.data = (unsigned char*)malloc(TEXTURE_SIZE_SQ);

	//Sort the images
	qsort((void*)images, c_images, sizeof(struct Image), compare);

	i = 0;
	for(;;) {

		//Populate the atlas
		memset(atlas.data + i_atlas * TEXTURE_SIZE_SQ, 0, TEXTURE_SIZE_SQ);
		populate(0, 0, TEXTURE_SIZE, TEXTURE_SIZE);

        //Write to file
        imageWrite(sz_output + "/" + util::toString(i++) + ".png", &atlas, sz_palette.empty() ? nullptr : palette);

		//See if there are any images left; if there aren't,
		//break out
		int done = 1;
		int j = 0;
		for(; j < c_images; j++)
			if(images[j].data) {
				done = 0;
				break;
			}
		if(done) {
			break;
		}
	}

    //Write to an info list
    {
        File file;
		std::string path = util::getPath(sz_output + "/atlas.list");
        if(!file.open(File::FILE_WRITE_NORMAL, path)) {
            std::cerr << "error: could not open file " << path << " for writing." << std::endl;
            code = 1;
            goto end;
        }

        file.writeWord(c_images);

        for(i = 0; i < c_images; i++) {
            //Write the name
            std::string name = fstr(images[i].name);
            file.writeByte(name.length());
            file.write(name.c_str(), name.length());
			
			//Write the position/size
			file.writeByte(images[i].atlas);
			file.writeWord(images[i].x);
			file.writeWord(images[i].y);
			file.writeWord(images[i].w);
			file.writeWord(images[i].h);

            //Write the shift
            file.writeWord(images[i].x_shift);
            file.writeWord(images[i].y_shift);
        }
    }

end:
	if(xy_images) {
		free(xy_images);
	}
	if(images) {
		for(i = 0; i < c_images; i++)
			if(images[i].data) {
				free(images[i].data);
			}
		delete [] images;
	}
	if(atlas.data) {
		free(atlas.data);
	}
	return code;
}
