#include "atlas.h"

#include <stdlib.h>

#include "globals.h"
#include "file.h"
#include "stage.h"
#include "graphics.h"

#define TEXTURE_SIZE 2048
#define TEXTURE_SIZE_SQ (TEXTURE_SIZE*TEXTURE_SIZE)

const int Atlas::channels[PIXEL_MAX] = {0, 1, 3, 4};

Atlas::Atlas()
{
    pixel_type = PIXEL_NULL;
    nImages = nSprites = 0;
    images = NULL;
    sprites = NULL;
}

Atlas::~Atlas()
{
    //if(textures) glDeleteTextures(c_textures, textures);

    delete [] sprites;
    delete [] images;
}

bool Atlas::create(std::string szFilename)
{
    File file;
    if(!file.open(FILE_READ_GZ, szFilename))
        return false;

    //Setup sprite buffers
    nSprites = file.readWord();
    sprites = new AtlasSprite[nSprites];

    //Read sprites
    for(int i = 0; i < nSprites; i++)
    {
        sprites[i].atlas = file.readByte();
        sprites[i].x = file.readWord();
        sprites[i].y = file.readWord();
        sprites[i].w = file.readWord();
        sprites[i].h = file.readWord();
    }

    //Setup pixel type
    pixel_type = file.readByte();

    //Setup pixel buffer, texture buffer
    nImages = file.readByte();
    images = new Image[nImages];
    ubyte_t* _b_pixel = (ubyte_t*)malloc(TEXTURE_SIZE_SQ * channels[pixel_type]);

    int _format = 0;
    switch(pixel_type)
    {
    case PIXEL_INDEXED:
        _format = COLORTYPE_INDEXED;
        break;
    case PIXEL_RGB:
        _format = COLORTYPE_RGB;
        break;
    case PIXEL_RGBA:
        _format = COLORTYPE_RGBA;
        break;
    };

    //Read each image and create each texture
    for(int i = 0; i < nImages; i++)
    {
        file.read(_b_pixel, TEXTURE_SIZE_SQ * channels[pixel_type]);

        //Create the image
        images[i].createFromMemory(_b_pixel, TEXTURE_SIZE, TEXTURE_SIZE, _format);

        //Generate the OpenGL texture
        /*glBindTexture(GL_TEXTURE_2D, textures[i]);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexImage2D(GL_TEXTURE_2D, 0, _format, TEXTURE_SIZE, TEXTURE_SIZE, 0, _format, GL_UNSIGNED_BYTE, _b_pixel);*/
    }

    //Free up memory
    free(_b_pixel);
    file.close();
    return true;
}

bool Atlas::createFromPalette(std::string szFilename, const ubyte_t* palette)
{
    File file;
    if(!file.open(FILE_READ_GZ, szFilename))
        return false;

    //Setup sprite buffers
    nSprites = file.readWord();
    sprites = new AtlasSprite[nSprites];

    //Read sprites
    for(int i = 0; i < nSprites; i++)
    {
        sprites[i].atlas = file.readByte();
        sprites[i].x = file.readWord();
        sprites[i].y = file.readWord();
        sprites[i].w = file.readWord();
        sprites[i].h = file.readWord();
    }

    //Setup pixel type
    pixel_type = file.readByte();
    if(pixel_type != PIXEL_INDEXED) return false;

    //Setup pixel buffer, texture buffer
    nImages = file.readByte();
    //textures = new unsigned int[c_textures];
    //glGenTextures(c_textures, textures);
    images = new Image[nImages];
    ubyte_t* _b_pixel = (ubyte_t*)malloc(TEXTURE_SIZE_SQ);

    //Read each image and create each texture
    for(int i = 0; i < nImages; i++)
    {
        file.read(_b_pixel, TEXTURE_SIZE_SQ);

        images[i].createFromMemoryPal(_b_pixel, TEXTURE_SIZE, TEXTURE_SIZE, palette);

        //Generate the OpenGL texture
        /*glBindTexture(GL_TEXTURE_2D, textures[i]);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, TEXTURE_SIZE, TEXTURE_SIZE, 0, GL_RGBA, GL_UNSIGNED_BYTE, _b_rgba);*/
    }

    //Free up memory
    free(_b_pixel);
    file.close();
    return true;
}

void Atlas::draw(int sprite_, int x_, int y_, bool mirror_)
{
    Graphics::setRect(sprites[sprite_].x, sprites[sprite_].y, sprites[sprite_].w, sprites[sprite_].h);
    images[sprites[sprite_].atlas].draw(x_, y_, mirror_);
}

void Atlas::drawSprite(int sprite_, int x_, int y_, bool mirror_)
{
    Graphics::setRect(sprites[sprite_].x, sprites[sprite_].y, sprites[sprite_].w, sprites[sprite_].h);
    images[sprites[sprite_].atlas].drawSprite(x_, y_, mirror_);
}

AtlasSprite Atlas::getSprite(int sprite_)
{
    if(sprite_ < 0 || sprite_ >= nSprites) return sprites[0];
    return sprites[sprite_];
}

AtlasSprite::AtlasSprite()
{
    atlas = 0;
    x = y = w = h = 0;
}
