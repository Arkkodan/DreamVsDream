#include "globals.h"
#include "video.h"
#include "error.h"
#include "graphics.h"

//Compressors enum, only used in this file
enum
{
    COMPRESSOR_UNSUPPORTED,
    COMPRESSOR_RAW,

    COMPRESSOR_MAX,
};

unsigned char* blankTex = NULL;

void Video::think()
{
    frameTimer += (float)FPS / avi->fps;

    //Generate the new texture
    if(frameTimer >= 1.0)
    {
        frameTimer -= 1.0;

        //Are we at the end of the avi?
        if(avi->video_pos >= avi->video_frames)
        {
            //Copy the blank buffer instead
            glBindTexture(GL_TEXTURE_2D, texture);
            glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, avi->width, avi->height, GL_RGB, GL_UNSIGNED_BYTE, blankTex);
            return;
        }

        //Update texture
        if(AVI_read_frame(avi, (char*)texBuff) == -1)
        {
            error("%s", AVI_strerror());
        }
        else
        {
            glBindTexture(GL_TEXTURE_2D, texture);
            glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, avi->width, avi->height, GL_BGR, GL_UNSIGNED_BYTE, texBuff);
        }
    }
}

void Video::draw(int x, int y)
{
    float w = (float)avi->width / (float)tw;
    float h = (float)avi->height / (float)th;

    float x1 = x;
    float y1 = y;
    float x2 = x1 + avi->width;
    float y2 = y1 + avi->height;

    //Set correct render mode
    glBlendEquation(GL_FUNC_ADD);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glBindTexture(GL_TEXTURE_2D, texture);
    glBegin(GL_QUADS);
    glTexCoord2f(0, h);
    glVertex3f(x1, y1, 0);
    glTexCoord2f(0, 0);
    glVertex3f(x1, y2, 0);
    glTexCoord2f(w, 0);
    glVertex3f(x2, y2, 0);
    glTexCoord2f(w, h);
    glVertex3f(x2, y1, 0);
    glEnd();
}

void Video::reset()
{
    AVI_seek_start(avi);
    frameTimer = FPS / avi->fps;
}

Video::Video(avi_t* _avi, char _compressor)
{
    avi = _avi;
    compressor = _compressor;

    //Generate a blank texture
    glGenTextures(1, &texture);
    tw = avi->width;
    th = avi->height;
    if(Graphics::force_POT)
    {
        tw = th = 1;
        while(tw < avi->width)
            tw *= 2;
        while(th < avi->height)
            th *= 2;
    }
    if(!blankTex)
    {
        blankTex = new unsigned char[tw * th * 3];
        memset(blankTex, 0, tw * th * 3);
    }
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, tw, th, 0, GL_RGB, GL_UNSIGNED_BYTE, blankTex);

    //Allocate the texture buff
    texBuff = new unsigned char[avi->width * avi->height * 3];

    frameTimer = FPS / avi->fps;
}

Video::~Video()
{
    delete [] texBuff;
    AVI_close(avi);
    glDeleteTextures(1, &texture);
}

Video* Video::create(const char* filename)
{
    avi_t* aviData = AVI_open_input_file(filename, 1);
    if(!aviData) return NULL;

    //Get some data and represent it more cleanly
    char compressor = COMPRESSOR_UNSUPPORTED;
    if(!strcasecmp(aviData->compressor, "DIB ") || !strcasecmp(aviData->compressor, "RAW ")
       || !memcmp(aviData->compressor, "\0\0\0\0", 4))
    {
        //Raw
        //char compressor = COMPRESSOR_RAW;
    }
    else
    {
        error("Unsupported codec: \"%s\"", aviData->compressor);
        AVI_close(aviData);
        return NULL;
    }

    return new Video(aviData, compressor);
}

/*
Video* Video::create(const char* filename)
{
    FILE* f = fopen(filename, "rb");
    if(!f) return NULL;

    fseek(f, 0, SEEK_END);
    size_t len = ftell(f);
    fseek(f, 0, SEEK_SET);

    char* data = new char[len];
    fread(data, len, 1, f);
    fclose(f);

    Video* vid = Video::createFromMemory(data);
    delete [] data;
    return vid;
}

unsigned int getInt(const char* data)
{
    uint32_t n = 0;
    n |= (uint32_t)data[0] >> 24;
    n |= (uint32_t)data[1] >> 16;
    n |= (uint32_t)data[2] >> 8;
    n |= (uint32_t)data[3];
    return n;
}


Video* Video::createFromMemory(const char* data)
{
    //Check the sigs
    if(memcmp(data, "RIFF", 4)) return NULL;

    data += 4;
    unsigned int size = getInt(data);

    data += 4;
    if(memcmp(data, "AVI ", 4)) return NULL;

    data += 4;
    if(memcmp(data, "LIST", 4)) return NULL;

    return NULL;
}
*/
