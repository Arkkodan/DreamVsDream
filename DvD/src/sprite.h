#ifndef SPRITE_H_INCLUDED
#define SPRITE_H_INCLUDED

#include <stdint.h>
#include <stddef.h>

#include "util.h"
#include "image.h"
#include "atlas.h"

class HitBox
{
public:
    Vector pos;
    Vector size;

    HitBox();
    HitBox(int x, int y, int w, int h);

    bool collideOther(HitBox* other, Vector* colpos);

    HitBox adjust(int _x, int _y, bool m, float scale);

    bool collidePoint(int pX, int pY);
    void draw(int _x, int _y, bool attack, bool selected);
};

class HitBoxGroup
{
public:
    int size;
    HitBox* boxes;

    HitBoxGroup();
    ~HitBoxGroup();

    void init(int _size);

#ifdef SPRTOOL
    HitBox* newHitbox();
    void deleteHitbox(HitBox* box);
#endif
};

enum
{
    HIT_NOT,
    HIT_HIT,
    HIT_ATTACK,
};

class Sprite
{
public:
    Sprite();
    ~Sprite();

#ifndef COMPILER
    void draw(int _x, int _y, bool mirror, float scale);
    void drawShadow(int _x, bool mirror, float scale);
#endif

    int collide(int x1, int y1, int x2, int y2, bool m1, bool m2, float scale1, float scale2, Sprite* other, Vector* colpos);

    int x;
    int y;

    //Each program has its own way of representing an image.
#ifdef SPRTOOL
    Image img;
#endif

#ifdef GAME
    Atlas* atlas;
    int atlas_sprite;
#else
    std::string name;
#endif
    //Image img;

    HitBoxGroup hitBoxes;
    HitBoxGroup aHitBoxes;
};

#ifdef GAME

enum
{
    EFFECT_NORMAL,
    EFFECT_SCATTER,
    EFFECT_HORIZONTAL,
    EFFECT_VERTICAL,
};

//Effects
class Effect
{
public:
    Effect();
    ~Effect();

    void init(int _frameC);
    void draw(int x, int y, int frame, bool mirror, int type, unsigned int inception);

    Image img;
    int frameC;
    int* frames;
    int* speeds;

    static void init();
    static void deinit();
};

class EffectGroup
{
public:
    EffectGroup();
    ~EffectGroup();

    void init(int _size);
    int id;
    int type;

    int size;
    Effect* effects;
};

//An in-game effect
class Spark
{
public:
    Spark();

    void think();
    void draw();

    int group;
    int effect;

    Vector pos;
    Vector vel;
    int frame;
    int timer;
    bool mirror;

    float alpha;
    unsigned int inception;

    /*ubyte_t r;
    ubyte_t g;
    ubyte_t b;*/
};

void newSpark(int x, int y, int group, bool mirror);
void newSpark(int x, int y, int vx, int vy, int group, bool mirror);
//void newSpark(int x, int y, int vx, int vy, int group, bool mirror, ubyte_t r, ubyte_t g, ubyte_t b);
void thinkSparks();
void drawSparks();

extern EffectGroup* effects;

#define SPARK_COUNT 64
extern Spark sparks[SPARK_COUNT];
#endif

#endif // SPRITE_H_INCLUDED
