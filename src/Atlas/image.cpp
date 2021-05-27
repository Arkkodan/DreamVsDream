#include "image.h"
#include "../../DvD/src/util.h"

#include <stdlib.h>
#include <png.h>
#include <string.h>
#include <string>
#include <iostream>

void imageRead(std::string szFileName_, struct Image* img_) {
	int i, j;

	FILE* f = nullptr;
	png_structp png_ptr = nullptr;
	png_infop info_ptr = nullptr;
	int w, h;
	int colortype;
	unsigned char* raw;
	png_bytep* row_pointers;

	//Initialize the image first
	img_->w = 0;
	img_->h = 0;
	img_->x_shift = 0;
	img_->y_shift = 0;
	img_->data = nullptr;

	int leftpad = 0, rightpad = 0, toppad = 0, bottompad = 0;

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
	png_read_png(png_ptr, info_ptr, PNG_TRANSFORM_STRIP_16 | PNG_TRANSFORM_PACKING, nullptr);

	//Get some basic info
	w = png_get_image_width(png_ptr, info_ptr);
	h = png_get_image_height(png_ptr, info_ptr);

	colortype = png_get_color_type(png_ptr, info_ptr);

	if(colortype != PNG_COLOR_TYPE_GRAY && colortype != PNG_COLOR_TYPE_PALETTE) {
		std::cerr << "error: file " << szFileName_ << " is not a grayscale or indexed image." << std::endl;
		goto end;
	}

	//Let's get the pixel data now, and free the png struct
	raw = (unsigned char*)malloc(w * h);
	row_pointers = png_get_rows(png_ptr, info_ptr);
	for(j = 0; j < h; j++)
		for(i = 0; i < w; i++) {
			raw[j * w + i] = row_pointers[j][i];
		}

	//Let's optimize the raw image. Remove empty space on all sides.
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
    img_->w = w - leftpad - rightpad;
    img_->h = h - toppad - bottompad;

    img_->data = (unsigned char*)malloc(img_->w * img_->h);
    for(j = 0; j < img_->h; j++) {
        for(i = 0; i < img_->w; i++) {
            img_->data[i + j * img_->w] = raw[leftpad + i + (toppad + j) * w];
        }
    }
    free(raw);

    img_->x_shift = leftpad;
    img_->y_shift = bottompad;

end:
	if(f) {
		fclose(f);
	}
	if(png_ptr || info_ptr) {
		png_free_data(png_ptr, info_ptr, PNG_FREE_ALL, -1);
		png_destroy_read_struct(&png_ptr, &info_ptr, nullptr);
	}
}

int imageReadPalette(std::string szFileName_, unsigned char* palette_) {
	int code = 0;

	FILE* f = nullptr;
	png_structp png_ptr = nullptr;
	png_infop info_ptr = nullptr;
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
	colortype = png_get_color_type(png_ptr, info_ptr);

	if(colortype != PNG_COLOR_TYPE_PALETTE) {
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
	if(png_ptr || info_ptr) {
		png_free_data(png_ptr, info_ptr, PNG_FREE_ALL, -1);
		png_destroy_read_struct(&png_ptr, &info_ptr, nullptr);
	}
	return code;
}

void imageWrite(std::string szFileName_, struct Image* img_, const unsigned char* palette_) {
	FILE* f = nullptr;
	png_structp png_ptr = nullptr;
	png_infop info_ptr = nullptr;


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

	//Write header
	png_set_IHDR(png_ptr, info_ptr, img_->w, img_->h,
	             8, palette_ ? PNG_COLOR_TYPE_PALETTE : PNG_COLOR_TYPE_GRAY, PNG_INTERLACE_NONE,
	             PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);
	if(palette_) {
		png_set_PLTE(png_ptr, info_ptr, (png_colorp)palette_, 256);
	}

	png_write_info(png_ptr, info_ptr);

	// Write image data
	for(int i = 0; i < img_->h; i++) {
		png_write_row(png_ptr, (png_bytep)(img_->data + img_->w * i));
	}

	// End write
	png_write_end(png_ptr, nullptr);

end:
	if(f) {
		fclose(f);
	}
	if(png_ptr || info_ptr) {
		png_free_data(png_ptr, info_ptr, PNG_FREE_ALL, -1);
		png_destroy_write_struct(&png_ptr, &info_ptr);
	}
}
