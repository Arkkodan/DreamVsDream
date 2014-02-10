#ifndef ANIMATION_H_INCLUDED
#define ANIMATION_H_INCLUDED

#include "globals.h"

#include "image.h"

class Animation {
public:
    Animation();
    Animation(Animation&& other);
    Animation& operator=(Animation&& other);
    ~Animation();

    Animation(const Animation& other) = delete;
    Animation& operator=(const Animation& other) = delete;

    //Load a GIF
    Animation(const std::string& filename);

    //Change/get status of animation
    void setPlaying(bool playing);
    bool isPlaying();

	void draw(int x, int y);

private:
    int nFrames;
    Image* frames;
    int* frameTimes;

    bool playing;
    unsigned int startFrame;
};

#endif
