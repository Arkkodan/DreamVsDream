#include <stdlib.h>
#include <math.h>

#ifdef EMSCRIPTEN
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <emscripten/emscripten.h>
#else
#include <png.h>
#endif

#include "image.h"
#include "sprite.h"
#include "globals.h"
#include "stage.h"
#include "graphics.h"

#include "player.h"

#include "error.h"

Image::Image() : w(0), h(0),
#ifdef COMPILER
	data(nullptr), xpad(0), ypad(0)
#else
	textures(nullptr), w_textures(0), h_textures(0),
	w_subtexture(0), h_subtexture(0)
#endif
{
}

Image::Image(Image&& other) {
    w = other.w;
    h = other.h;
#ifdef COMPILER
    xpad = other.xpad;
    ypad = other.ypad;
    data = other.data;
    other.data = nullptr;
#else
    w_textures = other.w_textures;
    h_textures = other.h_textures;
    w_subtexture = other.w_subtexture;
    h_subtexture = other.h_subtexture;
    textures = other.textures;
    other.textures = nullptr;
#endif
}

Image& Image::operator=(Image&& other) {
    w = other.w;
    h = other.h;

    using std::swap;
#ifdef COMPILER
    xpad = other.xpad;
    ypad = other.ypad;
    swap(data, other.data);
#else
    w_textures = other.w_textures;
    h_textures = other.h_textures;
    w_subtexture = other.w_subtexture;
    h_subtexture = other.h_subtexture;
    swap(textures, other.textures);
#endif
    return *this;
}

Image::~Image() {
#ifdef COMPILER
	free(data);
#else
	if(textures) {
		glDeleteTextures(w_textures * h_textures, textures);
		free(textures);
	}
#endif
}

#if defined EMSCRIPTEN

void image_loadPng(void* arg, const char* filename) {
	Image* img = (Image*)arg;
	img->createFromFile(std::string(filename));
}

void Image::createFromFile(std::string szPath) {
	SDL_Surface* surface = IMG_Load(szPath.c_str());
	if(!surface) {
		error("Cannot read \"" + szPath + "\".");
		goto end;
	}

	int format;
	switch(surface->format->BytesPerPixel) {
	case 3:
		format = COLORTYPE_RGB;
		break;
	case 4:
		format = COLORTYPE_RGBA;
		break;
	default:
		error("Not an RGB or RGBA png.");
		goto end;
	}
	createFromMemory((ubyte_t*)surface->pixels, surface->w, surface->h, format, nullptr);

	w = surface->w;
	h = surface->h;

end:
	if(surface) {
		SDL_FreeSurface(surface);
	}
}

void Image::createFromMemoryPNG(const ubyte_t* imgdata, size_t size) {
	emscripten_async_prepare_data((char*)imgdata, size, "png", (void*)this, image_loadPng, nullptr);
}

#elif defined COMPILER

void Image::createFromFile(std::string szPath) {
    std::string err;

	png_structp png_ptr = nullptr;
	png_infop info_ptr = nullptr;
	FILE* f = nullptr;
	png_bytep* row_pointers = nullptr;

	ubyte_t* raw = nullptr;

	unsigned int leftpad = 0, rightpad = 0, toppad = 0, bottompad = 0;

	f = util::ufopen(szPath, "rb");
	if(!f) {
		err = "Cannot read \"" + szPath + "\".";
		goto end;
	}

	//Check the header
	png_byte header[8];
	fread(header, 8, 1, f);
	if(png_sig_cmp(header, 0, 8) != 0) {
		err = "\"" + szPath + "\" not a valid PNG file.";
		goto end;
	}

	//Create
	png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);
	info_ptr = png_create_info_struct(png_ptr);
	if(!info_ptr) {
		goto error;
	}

	if(setjmp(png_jmpbuf(png_ptr))) {
		goto error;
	}

	png_init_io(png_ptr, f);
	png_set_sig_bytes(png_ptr, 8);
	png_read_png(png_ptr, info_ptr, PNG_TRANSFORM_IDENTITY, nullptr);

	//Basic info
	w = png_get_image_width(png_ptr, info_ptr);
	h = png_get_image_height(png_ptr, info_ptr);

	//This thing only returns indexed sprites, since its only being used by the compiler
	if(png_get_channels(png_ptr, info_ptr) != 1 || png_get_color_type(png_ptr, info_ptr) != PNG_COLOR_TYPE_PALETTE) {
        err = "\"" + szPath + "\" not an indexed PNG.";
		goto end;
	}

    //Let's get the pixel data now
	raw = (ubyte_t*)malloc(w * h);
	row_pointers = png_get_rows(png_ptr, info_ptr);
	for(unsigned int i = 0; i < h; i++)
		for(unsigned int j = 0; j < w; j++) {
			raw[j + i * w] = row_pointers[i][j];
		}

	//Let's optimize the raw image. Remove empty space on all sides.
	for(; leftpad < w; leftpad++) {
		bool found = false;
		for(unsigned int i = 0; i < h; i++)
			if(raw[leftpad + i * w]) {
				found = true;
				break;
			}
		if(found) {
			break;
		}
	}
	for(; rightpad < w; rightpad++) {
		bool found = false;
		for(unsigned int i = 0; i < h; i++)
			if(raw[((w - 1) - rightpad) + i * w]) {
				found = true;
				break;
			}
		if(found) {
			break;
		}
	}
	for(; toppad < h; toppad++) {
		bool found = false;
		for(unsigned int i = 0; i < w; i++)
			if(raw[i + toppad * w]) {
				found = true;
				break;
			}
		if(found) {
			break;
		}
	}
	for(; bottompad < h; bottompad++) {
		bool found = false;
		for(unsigned int i = 0; i < w; i++)
			if(raw[i + ((h - 1) - bottompad) * w]) {
				found = true;
				break;
			}
		if(found) {
			break;
		}
	}

	//Make a new image
	this->w = w - leftpad - rightpad;
	this->h = h - toppad - bottompad;

	data = (ubyte_t*)malloc(this->w * this->h);
	for(unsigned int y = 0; y < this->h; y++) {
		for(unsigned int x = 0; x < this->w; x++) {
			data[x + y * this->w] = raw[leftpad + x + (toppad + y) * w];
		}
	}

	xpad = leftpad;
	ypad = bottompad;

	goto end;

error:
	err = "Unknown error while reading PNG file \"" + szPath + "\".";

end:
	if(raw) {
		free(raw);
	}
	if(f) {
		fclose(f);
	}
	if(png_ptr || info_ptr) {
		png_free_data(png_ptr, info_ptr, PNG_FREE_ALL, -1);
		png_destroy_read_struct(&png_ptr, &info_ptr, nullptr);
	}

	if(!err.empty()) {
        die(err);
	}
}

#else

//For reading of PNGs from memory
struct pngStream {
	const ubyte_t* data;
	unsigned int offset;
	size_t size;
};

void pngRead(png_structp ptr, png_bytep data, png_size_t size) {
	pngStream* stream = (pngStream*)png_get_io_ptr(ptr);
	if(stream == nullptr) {
		return;
	}

	if(stream->offset + size > stream->size) {
		return;
	}

	memcpy(data, stream->data + stream->offset, size);
	stream->offset += size;
}

void Image::createFromFile(std::string szPath) {
    std::string err;

	png_structp png_ptr = nullptr;
	png_infop info_ptr = nullptr;
	FILE* f = nullptr;
	png_bytep* row_pointers = nullptr;
	int channels;
	int png_format;

	ubyte_t* data = nullptr;
	int width;
	int height;
	int format;

	int nPalette;
	png_colorp palette = nullptr;
	int nPalTrans;
	png_bytep palTrans = nullptr;

	f = util::ufopen(szPath, "rb");
	if(!f) {
		err = "Cannot read \"" + szPath + "\".";
		goto end;
	}

	//Check the header
	png_byte header[8];
	fread(header, 8, 1, f);
	if(png_sig_cmp(header, 0, 8) != 0) {
		err = "\"" + szPath + "\" not a valid PNG file.";
		goto end;
	}

	//Create
	png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);
	info_ptr = png_create_info_struct(png_ptr);
	if(!info_ptr) {
		goto error;
	}

	if(setjmp(png_jmpbuf(png_ptr))) {
		goto error;
	}

	png_init_io(png_ptr, f);
	png_set_sig_bytes(png_ptr, 8);
	png_read_png(png_ptr, info_ptr, PNG_TRANSFORM_STRIP_16 | PNG_TRANSFORM_EXPAND, nullptr);

	//Basic info
	width = png_get_image_width(png_ptr, info_ptr);
	height = png_get_image_height(png_ptr, info_ptr);

	channels = png_get_channels(png_ptr, info_ptr);
	png_format = png_get_color_type(png_ptr, info_ptr);

	//Check the colortype
	switch(png_format) {
    case PNG_COLOR_TYPE_GRAY:
        format = COLORTYPE_GRAYSCALE;
        break;

    case PNG_COLOR_TYPE_GRAY_ALPHA:
        format = COLORTYPE_GRAYSCALE_ALPHA;
        break;

	case PNG_COLOR_TYPE_RGB:
		format = COLORTYPE_RGB;
		break;

	case PNG_COLOR_TYPE_RGBA:
		format = COLORTYPE_RGBA;
		break;

	default:
		err = "\"" + szPath + "\" not a grayscale, indexed, RGB, or RGBA PNG.";
		goto end;
	}

	//Allocate the image buffer
	data = (ubyte_t*)malloc(width * height * channels);
	row_pointers = png_get_rows(png_ptr, info_ptr);
	for(int j = 0; j < height; j++)
		for(int i = 0; i < width; i++) {
            if(channels == 4 && row_pointers[j][i * channels + 3] == 0) {
                memset(data + (j * width + i) * channels, 0, channels);
            } else {
                memcpy(data + (j * width + i) * channels, row_pointers[j] + i * channels, channels);
            }
		}

    //If applicable, get the color palette
	if(format == COLORTYPE_INDEXED) {
        png_get_PLTE(png_ptr, info_ptr, &palette, &nPalette);
        png_get_tRNS(png_ptr, info_ptr, &palTrans, &nPalTrans, nullptr);
	}

#ifdef GAME
    createFromMemory(data, width, height, format);
#else
	createFromMemory(data, width, height, format);
#endif

	goto end;

error:
	err = "Unknown error while reading PNG file \"" + szPath + "\".";

end:
	if(data) {
		free(data);
	}
	if(f) {
		fclose(f);
	}
	if(png_ptr || info_ptr) {
		png_free_data(png_ptr, info_ptr, PNG_FREE_ALL, -1);
		png_destroy_read_struct(&png_ptr, &info_ptr, nullptr);
	}

	if(!err.empty()) {
        die(err);
	}
}

void Image::createFromMemoryPNG(const ubyte_t* imgdata, size_t size) {
    std::string err;

	png_structp png_ptr = nullptr;
	png_infop info_ptr = nullptr;
	png_bytep* row_pointers = nullptr;
	int channels;
	int png_format;

	//Stream
	pngStream stream;
	stream.data = imgdata;
	stream.offset = 8;
	stream.size = size;

	ubyte_t* data = nullptr;
	int width;
	int height;
	int format;

	int nPalette;
	png_colorp palette = nullptr;
	int nPalTrans;
	png_bytep palTrans = nullptr;

	//Check the header
	if(png_sig_cmp(imgdata, 0, 8) != 0) {
		err = "Embedded image not a valid PNG file.";
		goto end;
	}

	//Create
	png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);
	info_ptr = png_create_info_struct(png_ptr);
	if(!info_ptr) {
		goto error;
	}

	if(setjmp(png_jmpbuf(png_ptr))) {
		goto error;
	}

	png_set_read_fn(png_ptr, &stream, pngRead);
	png_set_sig_bytes(png_ptr, 8);
	png_read_png(png_ptr, info_ptr, PNG_TRANSFORM_STRIP_16 | PNG_TRANSFORM_EXPAND, nullptr);

	//Basic info
	width = png_get_image_width(png_ptr, info_ptr);
	height = png_get_image_height(png_ptr, info_ptr);

	channels = png_get_channels(png_ptr, info_ptr);
	png_format = png_get_color_type(png_ptr, info_ptr);

	//Check the colortype
	switch(png_format) {
    case PNG_COLOR_TYPE_GRAY:
        format = COLORTYPE_GRAYSCALE;
        break;

    case PNG_COLOR_TYPE_GRAY_ALPHA:
        format = COLORTYPE_GRAYSCALE_ALPHA;
        break;

	case PNG_COLOR_TYPE_RGB:
		format = COLORTYPE_RGB;
		break;

	case PNG_COLOR_TYPE_RGBA:
		format = COLORTYPE_RGBA;
		break;

	default:
		err = "Embedded image not a grayscale, indexed, RGB, or RGBA PNG.";
		goto end;
	}

	//Allocate the image buffer and copy data into it
	data = (ubyte_t*)malloc(width * height * channels);
	row_pointers = png_get_rows(png_ptr, info_ptr);
	for(int j = 0; j < height; j++)
		for(int i = 0; i < width * channels; i++) {
			data[j * (width * channels) + i] = row_pointers[j][i];
		}

    //If applicable, get the color palette
	if(format == COLORTYPE_INDEXED) {
        png_get_PLTE(png_ptr, info_ptr, &palette, &nPalette);
        png_get_tRNS(png_ptr, info_ptr, &palTrans, &nPalTrans, nullptr);
	}

	//Turn this data into textures!
#ifdef GAME
    createFromMemory(data, width, height, format);
#else
	createFromMemory(data, width, height, format);
#endif

	goto end;

error:
	err = "Unknown error while reading PNG file.";

end:
	if(png_ptr || info_ptr) {
		png_free_data(png_ptr, info_ptr, PNG_FREE_ALL, -1);
		png_destroy_read_struct(&png_ptr, &info_ptr, nullptr);
	}
	if(data) {
		free(data);
	}

	if(!err.empty()) {
        die(err);
	}
}
#endif

#ifndef COMPILER

//Take data and convert it into textures
void Image::createFromMemory(const ubyte_t* data_, unsigned int width_, unsigned int height_, int format_) {
#if 0
    //If we have an indexed image, transparently convert it to RGBA
    if(format_ == COLORTYPE_INDEXED && palette) {
        ubyte_t* _data_new = (ubyte_t*)malloc((palTrans ? 4 : 3) * width_ * height_);
        for(unsigned int j = 0; j < height_; j++) {
            for(unsigned int i = 0; i < width_; i++) {
                int offset = j * width_ + i;
                if(palTrans) {
                    if(data_[offset] < nPal) {
                        memcpy(&_data_new[offset * 4], &palette[data_[offset] * 3], 3);
                    } else {
                        memset(&_data_new[offset * 4], 0, 3);
                    }

                    if(data_[offset] < nPalTrans) {
                        _data_new[offset * 4 + 3] = palTrans[data_[offset] * 3];
                    } else {
                        _data_new[offset * 4 + 3] = 255;
                    }
                } else {
                    if(data_[offset] < nPal) {
                        memcpy(&_data_new[offset * 3], &palette[data_[offset] * 3], 3);
                    } else {
                        memset(&_data_new[offset * 3], 0, 3);
                    }
                }
            }
        }

        createFromMemory(_data_new, width_, height_, palTrans ? COLORTYPE_RGBA : COLORTYPE_RGB, nullptr, nullptr, 0, 0);
        free(_data_new);
        return;
    }
#endif

	//Determine channels
	int _format = 0;
	int _channels = 0;
	switch(format_) {
	case COLORTYPE_INDEXED:
    case COLORTYPE_GRAYSCALE:
		_format = GL_LUMINANCE;
		_channels = 1;
		break;
    case COLORTYPE_GRAYSCALE_ALPHA:
        _format = GL_LUMINANCE_ALPHA;
        _channels = 2;
        break;
	case COLORTYPE_RGB:
		_format = GL_RGB;
		_channels = 3;
		break;
	case COLORTYPE_RGBA:
		_format = GL_RGBA;
		_channels = 4;
		break;
	}

	//Size of textures before power-of-two mapping
	w = width_;
	h = height_;

	//Is the image smaller than the greatest texture size? Find the minimum size
	//that we need to resize it to.
	if(width_ < graphics::max_texture_size) {
		width_ = 1;
		while(width_ < w) {
			width_ *= 2;
		}
	}
	if(height_ < graphics::max_texture_size) {
		height_ = 1;
		while(height_ < h) {
			height_ *= 2;
		}
	}

	//What's the number of textures we need horizontally and vertically?
	w_textures = width_ / graphics::max_texture_size;
	h_textures = height_ / graphics::max_texture_size;
	if(width_ % graphics::max_texture_size) {
		w_textures++;
	}
	if(height_ % graphics::max_texture_size) {
		h_textures++;
	}

	if(w_textures == 1) {
		w_subtexture = width_;
	} else {
		w_subtexture = graphics::max_texture_size;
	}
	if(h_textures == 1) {
		h_subtexture = height_;
	} else {
		h_subtexture = graphics::max_texture_size;
	}

	//Create the buffer for the texture IDs and fill it
	textures = (unsigned int*)malloc(sizeof(unsigned int*) * (w_textures * h_textures));
	glGenTextures(w_textures * h_textures, textures);

	//Loop through each element and create a new texture
	ubyte_t* _b_texture = (ubyte_t*)malloc(w_subtexture * h_subtexture * _channels);
	for(unsigned int v = 0; v < h_textures; v++) {
		for(unsigned int u = 0; u < w_textures; u++) {
			//Copy the texture
			for(unsigned int j = 0; j < h_subtexture; j++) {
				for(unsigned int i = 0; i < w_subtexture * _channels; i++) {
					if(i < w * _channels && v * h_subtexture + j < h) {
						_b_texture[(j * w_subtexture * _channels) + i] = data_[((v * h_subtexture + j) * w * _channels) + u * w_subtexture * _channels + i];
					} else {
						_b_texture[(j * w_subtexture * _channels) + i] = 0;
					}
				}
			}

			//Create an OpenGL texture
			glBindTexture(GL_TEXTURE_2D, textures[v * w_textures + u]);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			glTexImage2D(GL_TEXTURE_2D, 0, _format, w_subtexture, h_subtexture, 0, _format, GL_UNSIGNED_BYTE, _b_texture);
		}
	}
	free(_b_texture);
}

void Image::draw(int x, int y, bool mirror) {
	if(!graphics::srcW || !graphics::srcH) {
		graphics::srcW = w;
		graphics::srcH = h;
	}

	//Set correct render mode
	if(graphics::render == RENDER_ADDITIVE || graphics::render == RENDER_SUBTRACTIVE) {
		glBlendFunc(GL_ONE, GL_ONE);
		if(graphics::render == RENDER_ADDITIVE) {
			glBlendEquation(GL_FUNC_ADD);
		} else if(graphics::render == RENDER_SUBTRACTIVE) {
			glBlendEquation(GL_FUNC_SUBTRACT);
		}
	} else if(graphics::render == RENDER_MULTIPLY) {
		glBlendFunc(GL_ZERO, GL_SRC_COLOR);
	}

	//How many textures does this thing span?
	int _x1_tex = graphics::srcX / w_subtexture;
	int _y1_tex = graphics::srcY / h_subtexture;
	int _x2_tex = (graphics::srcX + graphics::srcW - 1) / w_subtexture;
	int _y2_tex = (graphics::srcY + graphics::srcH - 1) / h_subtexture;

	float u1, v1, u2, v2, x1, y1, x2, y2;

	int _x_offset = w_subtexture - graphics::srcX % w_subtexture;
	int _y_offset = h_subtexture - graphics::srcY % h_subtexture;

	for(int v = _y1_tex; v <= _y2_tex; v++) {
		for(int u = _x1_tex; u <= _x2_tex; u++) {
			glBindTexture(GL_TEXTURE_2D, textures[v * w_textures + u]);

			if(u == _x1_tex) {
				u1 = (float)(graphics::srcX % w_subtexture) / (float)w_subtexture;
			} else {
				u1 = 0.0f;
			}
			if(v == _y1_tex) {
				v1 = (float)(graphics::srcY % h_subtexture) / (float)h_subtexture;
			} else {
				v1 = 0.0f;
			}
			if(u == _x2_tex) {
				u2 = (float)((graphics::srcX + graphics::srcW - 1) % w_subtexture + 1) / (float)w_subtexture;
			} else {
				u2 = 1.0f;
			}
			if(v == _y2_tex) {
				v2 = (float)((graphics::srcY + graphics::srcH - 1) % h_subtexture + 1) / (float)h_subtexture;
			} else {
				v2 = 1.0f;
			}

			if(mirror) {
				float temp;
				temp = u1;
				u1 = u2;
				u2 = temp;
			}

			//X1
			if(u > _x1_tex) {
				x1 = x + (_x_offset + (u-_x1_tex-1) * w_subtexture) * graphics::xscale;
			} else {
				x1 = x;
			}

			//Y1
			if(v > _y1_tex) {
				y1 = y + (_y_offset + (v-_y1_tex-1) * h_subtexture) * graphics::yscale;
			} else {
				y1 = y;
			}

			//X2
			if(u == _x2_tex) {
				x2 = x + graphics::srcW * graphics::xscale;
			} else if(u == _x1_tex) {
				x2 = x + _x_offset * graphics::xscale;
			} else {
				x2 = x1 + w_subtexture * graphics::xscale;
			}

			//Y2
			if(v == _y2_tex) {
				y2 = y + graphics::srcH * graphics::yscale;
			} else if(v == _y1_tex) {
				y2 = y + _y_offset * graphics::yscale;
			} else {
				y2 = y1 + h_subtexture * graphics::yscale;
			}

			if(mirror) {
				float _width = x2 - x1;
				x1 = x + graphics::srcW * graphics::xscale - (x1 - x) - _width;
				x2 = x1 + _width;
			}

			glBegin(GL_TRIANGLE_STRIP);

			//Top Left
			glTexCoord2f(u1, v1);
			glVertex3f(x1, y1, 0);

			//Bottom Left
			glTexCoord2f(u1, v2);
			glVertex3f(x1, y2, 0);

			//Bottom Right
			//glTexCoord2f(u2, v2);
			//glVertex3f(x2, y2, 0);

			//Top Left
			//glTexCoord2f(u1, v1);
			//glVertex3f(x1, y1, 0);

			//Top Right
			glTexCoord2f(u2, v1);
			glVertex3f(x2, y1, 0);

			//Bottom Right
			glTexCoord2f(u2, v2);
			glVertex3f(x2, y2, 0);

			glEnd();
		}
	}

	//Reset stuff
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
	graphics::srcX = graphics::srcY = graphics::srcW = graphics::srcH = 0;
	graphics::xscale = graphics::yscale = 1.0f;
	graphics::render = RENDER_NORMAL;
}

void Image::drawSprite(int x, int y, bool mirror) {
	if(!graphics::srcW || !graphics::srcH) {
		graphics::srcW = w;
		graphics::srcH = h;
	}
#ifdef SPRTOOL
	draw(x + WINDOW_WIDTH / 2, FLIP(y) - graphics::srcH * graphics::yscale, mirror);
#else
	extern util::Vector cameraPos;
	draw(x + WINDOW_WIDTH / 2 - cameraPos.x, FLIP(y) - graphics::srcH * graphics::yscale - STAGE.height + cameraPos.y, mirror);
#endif
}
#endif

bool Image::exists() {
#ifdef COMPILER
	return data != nullptr;
#else
	return textures != nullptr;
#endif
}

#define VAL_1X     255
#define VAL_2X     VAL_1X,  VAL_1X
#define VAL_4X     VAL_2X,  VAL_2X
#define VAL_8X     VAL_4X,  VAL_4X
#define VAL_16X    VAL_8X,  VAL_8X
#define VAL_32X    VAL_16X, VAL_16X
#define VAL_64X    VAL_32X, VAL_32X
#define VAL_128X   VAL_64X, VAL_64X

//Fucking brilliant
//http://stackoverflow.com/a/1565469/3037307
const ubyte_t Image::palTransFirst[256] = {0, VAL_128X, VAL_64X, VAL_32X, VAL_16X, VAL_8X, VAL_4X, VAL_2X, VAL_1X};
