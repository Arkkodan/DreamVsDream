#ifndef FIGHTER_H_INCLUDED
#define FIGHTER_H_INCLUDED

#include <stdint.h>
#include <limits.h>

#include "globals.h"
#include "sprite.h"
#include "image.h"
#include "sound.h"
#include "atlas.h"

enum
{
    SBOOL_UNDEFINED = -1,
    SBOOL_FALSE,
    SBOOL_TRUE,
};

#define CND_NOT (1<<7)

enum
{
    CND_NONE,
    CND_ON_GROUND,
    CND_CROUCHING,
    CND_AIRDASH,
    CND_DOUBLEJUMP,
};

class CommandTarget
{
public:
    CommandTarget();

    int state;
    int conditionC;
    ubyte_t conditions[16];
};

class Command
{
public:
    Command();
    ~Command();

    int comboC;
    uint16_t combo[16];
    uint16_t generic;
    int targetC;
    CommandTarget* targets;
};

class SoundGroup
{
public:
    SoundGroup();
    ~SoundGroup();

    void init(int _size);

    int size;
#ifdef COMPILER
    std::string name;
    std::string* sounds;
#else
    Sound* sounds;
#endif
};

class VoiceGroup
{
public:
    VoiceGroup();
    ~VoiceGroup();

    void init(int _size);

    int size;
    int pct;
#ifdef COMPILER
    std::string name;
    std::string* voices;
#else
    Voice* voices;
#endif
};

class State
{
public:
    State();
    ~State();

#ifdef COMPILER
    std::string name;
#endif

    unsigned int size;
    void* steps;
};

//State steps! Crazy stuff.
#define SVT_CONST 0
#define SVT_VAR 1
#define SVT_GLOBAL 2
class StepVar
{
public:
    byte_t type;
    float value;

    float getValue();
};

enum
{
    STEP_None,
    STEP_Sprite,
    STEP_Move,
    STEP_Scale,
    STEP_Type,
    STEP_Ctrl,
    STEP_Attack,
    STEP_Sound,
    STEP_Say,
    STEP_Heal,
    STEP_Special,
    STEP_Super,
    STEP_Force,
    STEP_Shoot,
    STEP_OnHit,
    STEP_Destroy,
    STEP_Visible,
    STEP_Invincible,
    STEP_Gravity,
    STEP_Airdash,
    STEP_DoubleJump,
    STEP_Mirror,
    STEP_Bounce,
    STEP_Hitspark,
    STEP_Cancel,
    STEP_Knockdown,
};

#define HT_LOW 0
#define HT_MID 1
#define HT_HIGH 2

struct StepAttack
{
    float damage;
    char type;
    float vX;
    float vY;
    int sound;
    bool cancel;
};

struct StepShoot
{
    int state;
    Vectorf force;
};

struct StepBounce
{
    Vectorf force;
    int pause;
};

/*
struct StepSprite
{
    int sprite;
    int wait;
};

struct StepMove
{
    float x;
    float y;
};

struct StepScale
{
    float scale;
};

struct StepType
{
    char type;
    char movetype;
};

struct StepCtrl
{
    bool ctrl;
};

struct StepSound
{
    int sound;
};

struct StepSay
{
    int voice;
};

struct StepHeal
{
    float heal;
};

struct StepSuper
{
    float meter;
};

struct StepSpecial
{
    bool ender;
};

struct StepForce
{
    Vectorf force;
};

struct StepOnHit
{
    int state;
};

struct StepDestroy
{
    char foo;
};

struct StepVisible
{
    bool visible;
};

struct StepInvincible
{
    bool invincible;
};

struct StepGravity
{
    bool gravity;
};

struct StepAirdash
{
    bool flag;
};

struct StepDoubleJump
{
    bool flag;
};

struct StepMirror
{
    bool flag;
};

struct StepHitspark
{
    int spark;
};

struct StepCancel
{
    int state;
};

struct StepKnockdown
{
    bool foo;
};
*/

enum
{
    STATE_STAND,
    STATE_CROUCH,
    STATE_BEGIN_STAND,
    STATE_BEGIN_CROUCH,

    STATE_WALK_FORWARD,
    STATE_WALK_BACK,

    STATE_CROUCH_WALK_FORWARD,
    STATE_CROUCH_WALK_BACK,

    STATE_DASH_FORWARD_BEGIN,
    STATE_DASH_FORWARD,
    STATE_DASH_FORWARD_END,
    STATE_DASH_BACK_BEGIN,
    STATE_DASH_BACK,
    STATE_DASH_BACK_END,

    STATE_JUMPING,
    STATE_BEGIN_FALL,
    STATE_FALLING,
    STATE_LAND,

    STATE_JTECH,

    STATE_HIT_HIGH,
    STATE_HIT_MID,
    STATE_HIT_LOW,

    STATE_BLOCK,
    STATE_CROUCH_BLOCK,

    STATE_KB,
    STATE_KB_FALL,
    STATE_KB_FALLING,
    STATE_KB_BOUNCE,
    STATE_KB_BOUNCING,
    STATE_KB_LAND,
    STATE_ON_BACK,
    STATE_RECOVER_BACK,

    STATE_KP,
    STATE_KP_FALL,
    STATE_KP_FALLING,
    STATE_KP_LAND,
    STATE_PRONE,
    STATE_RECOVER_PRONE,

    STATE_VICTORY,
    STATE_DEFEAT,

    STATE_MAX,
};

extern const char* stateNames[STATE_MAX];

#define STATE_NONE (UINT_MAX)

class Fighter
{
public:
    //General fighter stuff
    std::string name; //Data name
    std::string dname; //Display name
    int group;
    float defense;
    int height;
    int widthLeft;
    int widthRight;
    float gravity;

    //Sprites & palettes
    int c_palettes;
#ifdef COMPILER
    ubyte_t* palettes;
#else
    GLuint* palettes;
#endif
    int c_sprites;
    Sprite* sprites;
#ifdef GAME
    Atlas atlas_sprites;
#endif

    //Portraits
    Image select;
    Image portrait;
    Image portrait2;
    Image special;
    Image ender;
    Image portrait_ui;

#ifndef SPRTOOL
    //Sounds
    int c_sounds;
    SoundGroup* sounds;

    //Voices
    int c_voices;
    VoiceGroup* voices;

    //Commands
    int c_commands;
    Command* commands;

    //States
    int c_states;
    State* states;

    //Standard states
    unsigned int statesStandard[STATE_MAX];
#endif

    Fighter();
    explicit Fighter(std::string name);
    ~Fighter();

    void create(std::string name);

#ifdef SPRTOOL
    void saveSpr();
#endif

#ifndef COMPILER
    void draw(int sprite, int x, int y, bool mirror, float scale, unsigned int palette, float alpha, float r, float g, float b, float pct);
    void drawShadow(int sprite, int x, bool mirror, float scale);
#endif

#ifdef GAME
    //Init/deinit
    static void init();
    static void deinit();
#endif

protected:
};

#define FIGHTERS_MAX 3
extern Fighter fighters[FIGHTERS_MAX];

void setBool(bool& dst, char src);
void setStateBool(char& dst, bool src);

#endif // FIGHTER_H_INCLUDED
