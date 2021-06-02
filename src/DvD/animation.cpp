#include "animation.h"

// Use stb_image.h instead of gif_lib.h
// #include <gif_lib.h>
#define STB_IMAGE_IMPLEMENTATION
#define STBI_ONLY_GIF
#include <stb_image.h>

#include "error.h"
#include "util.h"

#include "os.h"

#include <fstream>

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
    // Set up local variables
    bool error = false;

    std::vector<char> buffer; // Contains gif file data
    size_t bufferSize;

    unsigned char* gif = nullptr; // Contains gif in GL ready format
    int* delays = nullptr; // Delays for each frame
    int width, height; // Spacial dimensions of the gif
    int comp; // Component count, stb_image always returns 4 for gif
    int stride; // One stride contains the data for one frame

	std::string path = util::getPath(filename);
    std::ifstream ifs(path, std::ios::binary);
    if (!ifs) {
        error = true;
        goto end;
    }

    ifs.seekg(0, ifs.end);
    bufferSize = static_cast<size_t>(ifs.tellg());
    ifs.seekg(0, ifs.beg);

    buffer.resize(bufferSize);
    ifs.read(buffer.data(), bufferSize);
    ifs.close();

    gif = stbi_load_gif_from_memory(
        reinterpret_cast<const stbi_uc*>(buffer.data()),
        static_cast<int>(bufferSize), &delays, &width, &height, &nFrames,
        &comp, STBI_default);
    if (!gif) {
        error = true;
        goto end;
    }

    // Check for unexpected behavior, expected to use COLORTYPE_RGBA
    if (comp != 4) {
        error = true;
        goto end;
    }

    stride = width * height * comp;

    //Allocate memory for gifs
    frames = new Image[nFrames];
    frameTimes = new int[nFrames];

    //Render all of the frames
    for(int i = 0; i < nFrames; i++) {
        //Create the Image
        frames[i].createFromMemory(&gif[i * stride], width, height, Image::COLORTYPE_RGBA, nullptr);
        // stb_image stores delays in units of 1/1000ths of a second
        frameTimes[i] = static_cast<int>(delays[i] * globals::FPS / 1000.0f);
    }

end:
    if (gif) {
        stbi_image_free(gif);
    }
    if (delays) {
        STBI_FREE(delays);
    }

    if(error) {
        error::die("Could not load GIF file \"" + path + "\"");
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
