#include "animation.h"

#include <gif_lib.h>

#include "error.h"
#include "util.h"

Animation::Animation() :
    nFrames(0), frames(nullptr), frameTimes(nullptr),
    playing(false), startFrame(0)
{}

Animation::Animation(Animation&& other) :
    nFrames(other.nFrames), frames(nullptr), frameTimes(nullptr),
    playing(other.playing), startFrame(other.startFrame)
{
    using std::swap;
    swap(frames, other.frames);
    swap(frameTimes, other.frameTimes);
}

Animation& Animation::operator=(Animation&& other) {
    nFrames = other.nFrames;
    playing = other.playing;
    startFrame = other.startFrame;

    using std::swap;
    swap(frames, other.frames);
    swap(frameTimes, other.frameTimes);
    return *this;
}

Animation::~Animation() {
    delete [] frames;
    delete [] frameTimes;
}

//Load a GIF
Animation::Animation(const std::string& filename) {
    bool error = false;

    FILE* f = nullptr;
    GifFileType* gif = nullptr;

    //pixel buffer for rendering gif
    ubyte_t* pixels = nullptr;

	std::string path = util::getPath(filename);
    f = util::ufopen(path, "rb");
    if(!f) {
        error = true;
        goto end;
    }
    gif = DGifOpenFileHandle(fileno(f), NULL);
    if(!gif || DGifSlurp(gif) != GIF_OK) {
        error = true;
        goto end;
    }

    //Allocate memory for gifs
    nFrames = gif->ImageCount;
    frames = new Image[nFrames];
    frameTimes = new int[nFrames];

    //Allocate and initialize render buffer
    pixels = new ubyte_t[gif->SWidth * gif->SHeight * 4];
    memset(pixels, 0, gif->SWidth * gif->SHeight * 4);

    //Render all of the frames
    for(int i = 0; i < nFrames; i++) {
		int transparent = -1;
		SavedImage* si = &gif->SavedImages[i];

		//Get the frame wait/transparent color, combine/replace
		for(int j = 0; j < si->ExtensionBlockCount; j++) {
			if(si->ExtensionBlocks[j].Function == GRAPHICS_EXT_FUNC_CODE) {
				GraphicsControlBlock gcb;
				DGifExtensionToGCB(si->ExtensionBlocks[j].ByteCount, si->ExtensionBlocks[j].Bytes, &gcb);
				frameTimes[i] = (gcb.DelayTime / (float)100) * FPS;
				if(!frameTimes[i])
                    frameTimes[i] = 1;
				transparent = gcb.TransparentColor;

				int clearColor = (transparent == -1 ? 0 : transparent);

				if(gcb.DisposalMode == DISPOSE_BACKGROUND || !i) {
					memset(pixels, clearColor, gif->SWidth * gif->SHeight * 4);
                } else if(gcb.DisposalMode == DISPOSE_PREVIOUS && i) {
                    SavedImage* siPrevious = &gif->SavedImages[i-1];
                    int bottom = siPrevious->ImageDesc.Top + siPrevious->ImageDesc.Height;
                    int right = siPrevious->ImageDesc.Left + siPrevious->ImageDesc.Width;

                    for(int y = siPrevious->ImageDesc.Top; y < bottom; y++) {
                        for(int x = siPrevious->ImageDesc.Left; x < right; x++) {
                            pixels[(y * gif->SWidth + x) * 4 + 3] = clearColor;
                        }
                    }
                }
				break;
			}
		}

		//Get the local or global colormap
		ColorMapObject* colormap;
		if(si->ImageDesc.ColorMap)
			colormap = si->ImageDesc.ColorMap;
		else
			colormap = gif->SColorMap;

		int bottom = si->ImageDesc.Top + si->ImageDesc.Height;
		int right = si->ImageDesc.Left + si->ImageDesc.Width;

		//Render GIF frame
		for (int y = si->ImageDesc.Top; y < bottom; y++) {
			for (int x = si->ImageDesc.Left; x < right; x++) {
				int index = si->RasterBits[(y - si->ImageDesc.Top) * si->ImageDesc.Width + (x - si->ImageDesc.Left)];
				if(index != transparent) {
                    GifColorType color = colormap->Colors[index];
					pixels[(y * gif->SWidth + x) * 4 + 0] = color.Red;
                    pixels[(y * gif->SWidth + x) * 4 + 1] = color.Green;
                    pixels[(y * gif->SWidth + x) * 4 + 2] = color.Blue;
                    pixels[(y * gif->SWidth + x) * 4 + 3] = 255;
				}
			}
		}

		//Create the Image
		frames[i].createFromMemory(pixels, gif->SWidth, gif->SHeight, COLORTYPE_RGBA, nullptr);
    }

end:
    if(pixels) {
        free(pixels);
    }

    if(gif) {
        DGifCloseFile(gif);
    }

    if(f) {
        fclose(f);
    }

    if(error) {
        die("Could not load GIF file \"" + path + "\"");
    }
}

void Animation::setPlaying(bool playing) {
    this->playing = playing;
    if(playing) {
        startFrame = os::frame;
    }
}

bool Animation::isPlaying() {
    return playing;
}

void Animation::draw(int x, int y) {
    int frame = 0;
    if(playing) {
        //Get the total number of frames
        int total = 0;
        for(int i = 0; i < nFrames; i++) {
            total += frameTimes[i];
        }

        //Determine the frame we must be on
        int time = (os::frame - startFrame) % total;
        for(int i = 0; i < nFrames; i++) {
            if(time < frameTimes[i]) {
                frame = i;
                break;
            }
            time -= frameTimes[i];
        }
    }

    //Draw the image!
    frames[frame].draw(x, y);
}
