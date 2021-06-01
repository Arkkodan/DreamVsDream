#ifndef EFFECT_H_INCLUDED
#define EFFECT_H_INCLUDED

#include "globals.h"

#include "image.h"
#include "player.h"

namespace effect {
    /// @brief Data entry containing animation of an effect
    class EffectAnimation {
    public:
        EffectAnimation();
        EffectAnimation(EffectAnimation&& other);
        EffectAnimation& operator=(EffectAnimation&& other);
        ~EffectAnimation();

        EffectAnimation(std::string name);

        //Do not copy anims
        EffectAnimation(const EffectAnimation& other) = delete;
        EffectAnimation& operator=(EffectAnimation& other) = delete;

        std::string getName();
        Image* getFrame(int frame);
        int getNumFrames();

    private:
        std::string name;
        Image* frames;
        int nFrames;
    };

    /// @brief In-game effect object
    class Effect {
    public:
        Effect();
        Effect(const std::string& name, int x, int y, bool moveWithCamera, bool mirror, int speed, int nLoops, game::Projectile* parent);
        ~Effect();

        unsigned int getCreationFrame();

        bool exists();
        void draw();

    private:
        EffectAnimation* anim;
        int speed;
        unsigned int frameStart;
        unsigned int frameEnd;

        int x, y;
        bool moveWithCamera;
        bool mirror;
		
		game::Projectile* parent;
    };

    /// @brief Create EffectAnimation objects
    void init();
    /// @brief Free EffectAnimation objects
    void deinit();

    /// @brief Create a new in-game effect
    void newEffect(const std::string& name, int x, int y, bool moveWithCamera, bool mirror, int speed, int nLoops, game::Projectile* parent);
    /// @brief Draw all in-game effects
    void draw();
}

#endif // EFFECT_H_INCLUDED
