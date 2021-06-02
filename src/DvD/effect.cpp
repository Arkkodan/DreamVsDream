#include "effect.h"

#include <algorithm>
#include <iostream>

#include "error.h"
#include "graphics.h"
#include "util.h"

#include "os.h"

namespace effect {
    constexpr auto EFFECT_MAX = 256;

    //VARIABLES
    int nEffectAnims = 0;
    EffectAnimation* effectAnims = nullptr;

    Effect effects[EFFECT_MAX];

    //EFFECT ANIMATION
    EffectAnimation::EffectAnimation() {
        name = "";
        nFrames = 0;
        frames = nullptr;
    }

    EffectAnimation::EffectAnimation(EffectAnimation&& other) {
        name = std::move(other.name);
        nFrames = other.nFrames;
        frames = other.frames;
        other.frames = nullptr;
    }

    EffectAnimation& EffectAnimation::operator=(EffectAnimation&& other) {
        nFrames = other.nFrames;
        using std::swap;
        swap(name, other.name);
        swap(frames, other.frames);
        return *this;
    }

    EffectAnimation::EffectAnimation(std::string _name) : name(std::move(_name)) {
        //Get the list of files
        std::vector<std::string> files = util::listDirectory(util::getPath("effects/" + name), true);

        nFrames = 0;
        frames = nullptr;

        if(files.empty()) {
            return;
        }

        //See if all of our images exist first, and count them.
        //Thumbs.db, .DS_Store, etc could screw the list up.
        for(std::vector<std::string>::size_type i = 0, last = -1; i < files.size(); i++) {
            if(std::find(files.begin(), files.end(), util::toString(i+1) + ".png") != files.end()) {
                if(last != i - 1) {
                    error::error("Missing frames in effect animation \"" + name + "\".");
                    return;
                }
                nFrames++;
                last++;
            }
        }

        if(!nFrames) {
            return;
        }

        //Allocate array and try to populate
        frames = new Image[nFrames];

        for(int i = 0; i < nFrames; i++) {
            frames[i].createFromFile("effects/" + name + "/" + util::toString(i+1) + ".png");
        }
    }

    EffectAnimation::~EffectAnimation() {
        delete [] frames;
    }

    std::string EffectAnimation::getName() {
        return name;
    }

    Image* EffectAnimation::getFrame(int frame) {
        if(nFrames == 0)
            return nullptr;

        return &frames[frame % nFrames];
    }

    int EffectAnimation::getNumFrames() {
        return nFrames;
    }

    //EFFECT
    Effect::Effect() {
        speed = frameStart = frameEnd = x = y = 0;
    }

    Effect::Effect(const std::string& name, int x, int y, bool moveWithCamera, bool mirror, int speed, int nLoops, game::Projectile* _parent) :
		parent(_parent)
	{
        anim = nullptr;

        //Look up the animation
        int i = 0;
        for(; i < nEffectAnims; i++) {
            if(name == effectAnims[i].getName()) {
                anim = effectAnims + i;
                break;
            }
        }

        //We failed to find the animation
        if(i == nEffectAnims) {
            error::error("Could not find the effect animation \"" + name + "\".");
            this->x = this->y = this->frameEnd = this->frameStart = 0;
        } else {
            //Create object
            this->x = x;
            this->y = y;
            this->moveWithCamera = moveWithCamera;
            this->mirror = mirror;
            this->speed = speed;

            //Calculate the start and end frames
            this->frameStart = os::frame;
            this->frameEnd = this->frameStart + (unsigned int)(anim->getNumFrames() * nLoops * speed);
        }
    }

    Effect::~Effect() {
    }

    unsigned int Effect::getCreationFrame() {
        return frameStart;
    }

    bool Effect::exists() {
        return frameEnd > os::frame;
    }

    void Effect::draw() {
        if(!exists())
            return;

        Image* frame = anim->getFrame((os::frame - frameStart) / speed);
		
		int x1 = x - frame->w / 2;
		int y1 = y - frame->h / 2;
		
		if(parent) {
			x1 += parent->pos.x;
			y1 += parent->pos.y;
		}

        graphics::setRender(Image::RENDER_ADDITIVE);
        if(moveWithCamera)
            frame->drawSprite(x1, y1, mirror);
        else
            frame->draw(x1, y1, mirror);
    }

    //MISC FUNCS
    void init() {
        //See what's in the effects directory
        std::vector<std::string> dirs = util::listDirectory(util::getPath("effects"), false);

        nEffectAnims = dirs.size();
        if(nEffectAnims <= 0)
            return;

        effectAnims = new EffectAnimation[nEffectAnims];
        for(int i = 0; i < nEffectAnims; i++) {
            effectAnims[i] = EffectAnimation(dirs[i]);
        }
    }

    void deinit() {
        delete [] effectAnims;
    }

    void newEffect(const std::string& name, int x, int y, bool moveWithCamera, bool mirror, int speed, int nLoops, game::Projectile* parent) {
        for(int i = 0; i < EFFECT_MAX; i++) {
            if(!effects[i].exists()) {
                effects[i] = Effect(name, x, y, moveWithCamera, mirror, speed, nLoops, parent);
                break;
            }
        }
    }

    void draw() {
        //Be sure to draw all the effects in the correct order
        bool drawn[EFFECT_MAX] = {false};

        for(;;) {
            int iEarliest = -1;
            unsigned int earliest = UINT_MAX;
            for(int i = 0; i < EFFECT_MAX; i++) {
                if(!drawn[i] && effects[i].exists() && effects[i].getCreationFrame() < earliest) {
                    iEarliest = i;
                    earliest = effects[i].getCreationFrame();
                }
            }
            if(iEarliest == -1)
                break;
            drawn[iEarliest] = true;

            effects[iEarliest].draw();
        }
    }
}
