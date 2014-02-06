#include "image.h"
#include "../../DvD/src/util.h"

#include <stdlib.h>
#include <png.h>
#include <string.h>
#include <string>
#include <iostream>

int channels[PIXEL_MAX] = {0, 1, 3, 4};

int imageRead(std::string szFileName_, struct Image* img_, int type_) {
	int i, j;

	FILE* f = nullptr;
	png_structp png_ptr = nullptr;
	png_infop info_ptr = nullptr;
	int w, h;
	int bitdepth, channels;
	int colortype = -1;
	int type_png = -1;
	unsigned char* raw;
	png_bytep* row_pointers;

	//Initialize the image first
	img_->w = 0;
	img_->h = 0;
	img_->x_shift = 0;
	img_->y_shift = 0;
	img_->data = nullptr;

	f = util::ufopen(szFileName_, "rb");
	if(!f) {
		std::cerr << "error: cannot read \"" << szFileName_ << "\"." << std::endl;
		goto end;
	}

	//Check the header
	png_byte header[8];
	fread(header, 8, 1, f);
	if(png_sig_cmp(header, 0, 8) != 0) {
		goto end;
	}

	//Create read struct
	png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);
	info_ptr = png_create_info_struct(png_ptr);
	if(!info_ptr) {
		goto end;
	}

	if(setjmp(png_jmpbuf(png_ptr))) {
		goto end;
	}

	//Read the PNG from the file, close the file
	png_init_io(png_ptr, f);
	png_set_sig_bytes(png_ptr, 8);
	png_read_png(png_ptr, info_ptr, PNG_TRANSFORM_IDENTITY, nullptr);

	//Get some basic info
	w = png_get_image_width(png_ptr, info_ptr);
	h = png_get_image_height(png_ptr, info_ptr);

	bitdepth = png_get_bit_depth(png_ptr, info_ptr);
	channels = png_get_channels(png_ptr, info_ptr);
	colortype = png_get_color_type(png_ptr, info_ptr);

	//Load only the defined type
	switch(type_) {
	case PIXEL_INDEXED:
		type_png = PNG_COLOR_TYPE_PALETTE;
		break;
	case PIXEL_RGB:
		type_png = PNG_COLOR_TYPE_RGB;
		break;
	case PIXEL_RGBA:
		type_png = PNG_COLOR_TYPE_RGBA;
		break;
	}

	if(bitdepth != 8 || (type_png != -1 && colortype != type_png)) {
		std::cerr << "error: file " << szFileName_ << " uses an incorrect pixel type." << std::endl;
		goto end;
	}

	//Let's get the pixel data now, and free the png struct
	raw = (unsigned char*)malloc(w * h * channels);
	row_pointers = png_get_rows(png_ptr, info_ptr);
	for(j = 0; j < h; j++)
		for(i = 0; i < w * channels; i++) {
			raw[j * w * channels + i] = row_pointers[j][i];
		}

	//Let's optimize the raw image. Remove empty space on all sides.
	if(colortype == PNG_COLOR_TYPE_PALETTE) {
		int leftpad = 0;
		for(; leftpad < w; leftpad++) {
			int found = 0;
			for(i = 0; i < h; i++)
				if(raw[leftpad + i * w]) {
					found = 1;
					break;
				}
			if(found) {
				break;
			}
		}
		int rightpad = 0;
		for(; rightpad < w; rightpad++) {
			int found = 0;
			for(i = 0; i < h; i++)
				if(raw[((w - 1) - rightpad) + i * w]) {
					found = 1;
					break;
				}
			if(found) {
				break;
			}
		}
		int toppad = 0;
		for(; toppad < h; toppad++) {
			int found = 0;
			for(i = 0; i < w; i++)
				if(raw[i + toppad * w]) {
					found = 1;
					break;
				}
			if(found) {
				break;
			}
		}
		int bottompad = 0;
		for(; bottompad < h; bottompad++) {
			int found = 0;
			for(i = 0; i < w; i++)
				if(raw[i + ((h - 1) - bottompad) * w]) {
					found = 1;
					break;
				}
			if(found) {
				break;
			}
		}

		//Make a new image
		int newWidth = w - leftpad - rightpad;
		int newHeight = h - toppad - bottompad;

		unsigned char* data = (unsigned char*)malloc(newWidth * newHeight);
		for(j = 0; j < newHeight; j++) {
			for(i = 0; i < newWidth; i++) {
				data[i + j * newWidth] = raw[leftpad + i + (toppad + j) * w];
			}
		}
		free(raw);

		img_->w = newWidth;
		img_->h = newHeight;
		img_->x_shift = leftpad;
		img_->y_shift = bottompad;
		img_->data = data;
	} else if(colortype == PNG_COLOR_TYPE_RGBA) {
		int leftpad = 0;
		for(; leftpad < w; leftpad++) {
			int found = 0;
			for(i = 0; i < h; i++)
				if(raw[(leftpad + i * w) * 4 + 3]) {
					found = 1;
					break;
				}
			if(found) {
				break;
			}
		}
		int rightpad = 0;
		for(; rightpad < w; rightpad++) {
			int found = 0;
			for(i = 0; i < h; i++)
				if(raw[(((w - 1) - rightpad) + i * w) * 4 + 3]) {
					found = 1;
					break;
				}
			if(found) {
				break;
			}
		}
		int toppad = 0;
		for(; toppad < h; toppad++) {
			int found = 0;
			for(i = 0; i < w; i++)
				if(raw[(i + toppad * w) * 4 + 3]) {
					found = 1;
					break;
				}
			if(found) {
				break;
			}
		}
		int bottompad = 0;
		for(; bottompad < h; bottompad++) {
			int found = 0;
			for(i = 0; i < w; i++)
				if(raw[(i + ((h - 1) - bottompad) * w) * 4 + 3]) {
					found = 1;
					break;
				}
			if(found) {
				break;
			}
		}

		//Make a new image
		int newWidth = w - leftpad - rightpad;
		int newHeight = h - toppad - bottompad;

		unsigned char* data = (unsigned char*)malloc(newWidth * newHeight * 4);
		for(j = 0; j < newHeight; j++) {
			for(i = 0; i < newWidth * channels; i++) {
				data[i + j * newWidth * 4] = raw[leftpad * 4 + i + (toppad + j) * w * 4];
			}
		}
		free(raw);

		img_->w = newWidth;
		img_->h = newHeight;
		img_->x_shift = leftpad;
		img_->y_shift = toppad;
		img_->data = data;
	} else {
		img_->w = w;
		img_->h = h;
		img_->data = raw;
	}

end:
	if(f) {
		fclose(f);
	}
	if(info_ptr) {
		png_free_data(png_ptr, info_ptr, PNG_FREE_ALL, -1);
	}
	if(png_ptr) {
		png_destroy_read_struct(&png_ptr, (png_infopp)nullptr, (png_infopp)nullptr);
	}

	switch(colortype) {
	case PNG_COLOR_TYPE_PALETTE:
		return PIXEL_INDEXED;
	case PNG_COLOR_TYPE_RGB:
		return PIXEL_RGB;
	case PNG_COLOR_TYPE_RGBA:
		return PIXEL_RGBA;
	default:
		return PIXEL_NULL;
	}
}

int imageReadPalette(std::string szFileName_, unsigned char* palette_) {
	int code = 0;

	FILE* f = nullptr;
	png_structp png_ptr = nullptr;
	png_infop info_ptr = nullptr;
	int bitdepth;
	int colortype = -1;
	png_colorp palette = nullptr;
	int c_palette = 0;

	f = util::ufopen(szFileName_, "rb");
	if(!f) {
		std::cerr << "error: cannot read \"" << szFileName_ << "\"." << std::endl;
		code = 1;
		goto end;
	}

	//Check the header
	png_byte header[8];
	fread(header, 8, 1, f);
	if(png_sig_cmp(header, 0, 8) != 0) {
		goto end;
	}

	//Create read struct
	png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);
	info_ptr = png_create_info_struct(png_ptr);
	if(!info_ptr) {
		goto end;
	}

	if(setjmp(png_jmpbuf(png_ptr))) {
		goto end;
	}

	//Read the PNG from the file, close the file
	png_init_io(png_ptr, f);
	png_set_sig_bytes(png_ptr, 8);
	png_read_png(png_ptr, info_ptr, PNG_TRANSFORM_IDENTITY, nullptr);

	//Get some basic info
	bitdepth = png_get_bit_depth(png_ptr, info_ptr);
	colortype = png_get_color_type(png_ptr, info_ptr);

	if(bitdepth != 8 || colortype != PNG_COLOR_TYPE_PALETTE) {
		std::cerr << "error: file " << szFileName_ << " uses an incorrect pixel type." << std::endl;
		code = 1;
		goto end;
	}

	png_get_PLTE(png_ptr, info_ptr, &palette, &c_palette);

	if(c_palette != 256) {
		std::cerr << "error: invalid palette size: " << c_palette << std::endl;
		code = 1;
		goto end;
	}

	memcpy(palette_, palette, 256 * 3);

end:
	if(f) {
		fclose(f);
	}
	if(info_ptr) {
		png_free_data(png_ptr, info_ptr, PNG_FREE_ALL, -1);
	}
	if(png_ptr) {
		png_destroy_read_struct(&png_ptr, (png_infopp)nullptr, (png_infopp)nullptr);
	}
	return code;
}

void imageWrite(std::string szFileName_, struct Image* img_, int type_, const unsigned char* palette_) {
	FILE* f = nullptr;
	png_structp png_ptr = nullptr;
	png_infop info_ptr = nullptr;
	int type_png = 0;


	//Open file for writing (binary mode)
	f = util::ufopen(szFileName_, "wb");
	if(f == nullptr) {
		std::cerr << "error: cannot write to \"" << szFileName_ << "\"." << std::endl;
		goto end;
	}

	//Initialize write structure
	png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);
	if (png_ptr == nullptr) {
		goto end;
	}

	//Initialize info structure
	info_ptr = png_create_info_struct(png_ptr);
	if(info_ptr == nullptr) {
		goto end;
	}

	//Setup Exception handling
	if(setjmp(png_jmpbuf(png_ptr))) {
		goto end;
	}

	png_init_io(png_ptr, f);

	switch(type_) {
	case PIXEL_INDEXED:
		type_png = PNG_COLOR_TYPE_PALETTE;
		break;
	case PIXEL_RGB:
		type_png = PNG_COLOR_TYPE_RGB;
		break;
	case PIXEL_RGBA:
		type_png = PNG_COLOR_TYPE_RGBA;
		break;
	default:
		goto end;
	}

	//Write header
	png_set_IHDR(png_ptr, info_ptr, img_->w, img_->h,
	             8, type_png, PNG_INTERLACE_NONE,
	             PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);
	if(type_ == PIXEL_INDEXED) {
		png_set_PLTE(png_ptr, info_ptr, (png_colorp)palette_, 256);
	}

	// Set title
	/*if (title != nullptr) {
		png_text title_text;
		title_text.compression = PNG_TEXT_COMPRESSION_NONE;
		title_text.key = "Title";
		title_text.text = title;
		png_set_text(png_ptr, info_ptr, &title_text, 1);
	}*/

	png_write_info(png_ptr, info_ptr);

	// Write image data
	for(int i = 0; i < img_->h; i++) {
		png_write_row(png_ptr, (png_bytep)(img_->data + img_->w * i * channels[type_]));
	}

	// End write
	png_write_end(png_ptr, nullptr);

end:
	if(f) {
		fclose(f);
	}
	if(info_ptr) {
		png_free_data(png_ptr, info_ptr, PNG_FREE_ALL, -1);
	}
	if(png_ptr) {
		png_destroy_write_struct(&png_ptr, (png_infopp)nullptr);
	}
}
