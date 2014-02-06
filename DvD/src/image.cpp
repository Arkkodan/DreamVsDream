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

#ifndef COMPILER
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

void Image::createFromFile(std::string szPath) {
#ifdef GAME
#ifdef EMSCRIPTEN
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
	createFromMemory((ubyte_t*)surface->pixels, surface->w, surface->h, format);

	w = surface->w;
	h = surface->h;

end:
	if(surface) {
		SDL_FreeSurface(surface);
	}

#else
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

	f = util::ufopen(szPath, "rb");
	if(!f) {
		error("Cannot read \"" + szPath + "\".");
		goto end;
	}

	//Check the header
	png_byte header[8];
	fread(header, 8, 1, f);
	if(png_sig_cmp(header, 0, 8) != 0) {
		error("\"" + szPath + "\" not a valid PNG file.");
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
	width = png_get_image_width(png_ptr, info_ptr);
	height = png_get_image_height(png_ptr, info_ptr);

	//int bitdepth = png_get_bit_depth(png_ptr, info_ptr);
	channels = png_get_channels(png_ptr, info_ptr);
	png_format = png_get_color_type(png_ptr, info_ptr);

	//Check the colortype
	switch(png_format) {
	case PNG_COLOR_TYPE_RGB:
		format = COLORTYPE_RGB;
		break;

	case PNG_COLOR_TYPE_RGBA:
		format = COLORTYPE_RGBA;
		break;

	default:
		error("Not an RGB or RGBA png.");
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

	createFromMemory(data, width, height, format);

	goto end;

error:
	error("Unknown error while reading PNG file \"" + szPath + "\".");

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
#endif
#else
	FILE* fp = util::ufopen(szPath, "rb");
	if(!fp) {
		error("Cannot read \"" + szPath + "\".");
		return;
	}

	//Check the header
	png_byte header[8];
	fread(header, 8, 1, fp);
	if(png_sig_cmp(header, 0, 8) != 0) {
		return;
	}

	png_structp png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);
	png_infop info_ptr = png_create_info_struct(png_ptr);
	if(!info_ptr) {
		png_destroy_read_struct(&png_ptr, nullptr, nullptr);
		return;
	}

	if(setjmp(png_jmpbuf(png_ptr))) {
		png_destroy_read_struct(&png_ptr, &info_ptr, nullptr);
		if(fp) {
			fclose(fp);
		}
		return;
	}

	png_init_io(png_ptr, fp);
	png_set_sig_bytes(png_ptr, 8);
	png_read_png(png_ptr, info_ptr, PNG_TRANSFORM_IDENTITY, nullptr);
	fclose(fp);

	//Basic info
	w = png_get_image_width(png_ptr, info_ptr);
	h = png_get_image_height(png_ptr, info_ptr);

	int bitdepth = png_get_bit_depth(png_ptr, info_ptr);
	int channels = png_get_channels(png_ptr, info_ptr);
	int colortype = png_get_color_type(png_ptr, info_ptr);

	//This thing only returns indexed sprites, since its only being used by the compiler
	if(bitdepth != 8 || channels != 1 || colortype != PNG_COLOR_TYPE_PALETTE) {
		png_destroy_read_struct(&png_ptr, &info_ptr, nullptr);
		return;
	}

	//Let's get the pixel data now
	ubyte_t* raw = (ubyte_t*)malloc(w * h);
	png_bytep* row_pointers = png_get_rows(png_ptr, info_ptr);
	for(unsigned int i = 0; i < h; i++)
		for(unsigned int j = 0; j < w; j++) {
			raw[j + i * w] = row_pointers[i][j];
		}

#ifdef COMPILER
	//Let's optimize the raw image. Remove empty space on all sides.
	unsigned int leftpad = 0;
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
	unsigned int rightpad = 0;
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
	unsigned int toppad = 0;
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
	unsigned int bottompad = 0;
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
	int newWidth = w - leftpad - rightpad;
	int newHeight = h - toppad - bottompad;

	data = (ubyte_t*)malloc(newWidth * newHeight);
	for(int y = 0; y < newHeight; y++) {
		for(int x = 0; x < newWidth; x++) {
			data[x + y * newWidth] = raw[leftpad + x + (toppad + y) * w];
		}
	}
	free(raw);
	png_destroy_read_struct(&png_ptr, &info_ptr, nullptr);

	w = newWidth;
	h = newHeight;
	xpad = leftpad;
	ypad = bottompad;
#endif

#ifdef SPRTOOL
	//And now the palette
	int num_palette;
	png_colorp palette;
	png_get_PLTE(png_ptr, info_ptr, &palette, &num_palette);

	ubyte_t* data = (ubyte_t*)malloc(w * h * 4);
	for(unsigned int y = 0; y < h; y++) {
		for(unsigned int x = 0; x < w; x++) {
			data[x * 4 + y * w * 4 + 0] = palette[raw[x + y * w]].red;
			data[x * 4 + y * w * 4 + 1] = palette[raw[x + y * w]].green;
			data[x * 4 + y * w * 4 + 2] = palette[raw[x + y * w]].blue;
			if(raw[x + y * w]) {
				data[x * 4 + y * w * 4 + 3] = 255;
			} else {
				data[x * 4 + y * w * 4 + 3] = 0;
			}
		}
	}
	png_destroy_read_struct(&png_ptr, &info_ptr, nullptr);
	createFromMemory(data, w, h, COLORTYPE_RGBA);
	free(raw);
	free(data);
#endif
#endif
}

#ifndef EMSCRIPTEN
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
#endif

#ifdef EMSCRIPTEN
void image_loadPng(void* arg, const char* filename) {
	Image* img = (Image*)arg;
	img->createFromFile(std::string(filename));
}
#endif

#ifndef COMPILER
void Image::createFromMemoryPNG(ubyte_t* imgdata, size_t size) {
#ifdef EMSCRIPTEN
	emscripten_async_prepare_data((char*)imgdata, size, "png", (void*)this, image_loadPng, nullptr);
#else
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

	//Check the header
	if(png_sig_cmp(imgdata, 0, 8) != 0) {
		error("Not a valid PNG file.");
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
	png_read_png(png_ptr, info_ptr, PNG_TRANSFORM_IDENTITY, nullptr);

	//Basic info
	width = png_get_image_width(png_ptr, info_ptr);
	height = png_get_image_height(png_ptr, info_ptr);

	channels = png_get_channels(png_ptr, info_ptr);
	png_format = png_get_color_type(png_ptr, info_ptr);

	//Check the colortype
	switch(png_format) {
	case PNG_COLOR_TYPE_RGB:
		format = COLORTYPE_RGB;
		break;

	case PNG_COLOR_TYPE_RGBA:
		format = COLORTYPE_RGBA;
		break;

	default:
		error("Not an RGB or RGBA png.");
		goto end;
	}

	//Allocate the image buffer and copy data into it
	data = (ubyte_t*)malloc(width * height * channels);
	row_pointers = png_get_rows(png_ptr, info_ptr);
	for(int j = 0; j < height; j++)
		for(int i = 0; i < width * channels; i++) {
			data[j * (width * channels) + i] = row_pointers[j][i];
		}

	//Turn this data into textures!
	createFromMemory(data, width, height, format);

	goto end;

error:
	error("Unknown error while reading PNG file.");

end:
	if(png_ptr || info_ptr) {
		png_free_data(png_ptr, info_ptr, PNG_FREE_ALL, -1);
		png_destroy_read_struct(&png_ptr, &info_ptr, nullptr);
	}
	if(data) {
		free(data);
	}
#endif
}

void Image::createFromMemoryPal(ubyte_t* imgdata, unsigned int width, unsigned int height, const ubyte_t* palette) {
	ubyte_t* _data_new = (ubyte_t*)malloc(4 * width * height);
	for(unsigned int j = 0; j < height; j++)
		for(unsigned int i = 0; i < width; i++) {
			if(imgdata[j * width + i]) {
				_data_new[(j*width+i)*4+0] = palette[imgdata[j*width+i]*3+0];
				_data_new[(j*width+i)*4+1] = palette[imgdata[j*width+i]*3+1];
				_data_new[(j*width+i)*4+2] = palette[imgdata[j*width+i]*3+2];
				_data_new[(j*width+i)*4+3] = 255;
			} else {
				_data_new[(j*width+i)*4+0] = 0;
				_data_new[(j*width+i)*4+1] = 0;
				_data_new[(j*width+i)*4+2] = 0;
				_data_new[(j*width+i)*4+3] = 0;
			}
		}

	createFromMemory(_data_new, width, height, COLORTYPE_RGBA);
	free(_data_new);
}

//Take data and convert it into textures
void Image::createFromMemory(ubyte_t* data_, unsigned int width_, unsigned int height_, int format_) {
	//Determine channels
	int _format = 0;
	int _channels = 0;
	switch(format_) {
	case COLORTYPE_INDEXED:
		_format = GL_LUMINANCE;
		_channels = 1;
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
#endif

bool Image::exists() {
#ifdef COMPILER
	return data != nullptr;
#else
	return textures != nullptr;
#endif
}
