#ifndef DVD_PLAYER_H
#define DVD_PLAYER_H

#ifndef COMPILER
#include "fighter.h"
#include "sound.h"

#include <glm/vec2.hpp>

#include <array>
#include <cstdint>
#include <string>
#endif // COMPILER

namespace game {
  constexpr auto INPUT_DIRMASK = 0xF;
  constexpr auto INPUT_KEYMASK = 0x70;
  constexpr auto INPUT_PRESSMASK = 0x00FF;
  constexpr auto INPUT_RELMASK = 0xFF00;
  constexpr auto INPUT_RELSHIFT = 8;

  constexpr auto INPUT_LEFT = 1 << 0;
  constexpr auto INPUT_RIGHT = 1 << 1;
  constexpr auto INPUT_UP = 1 << 2;
  constexpr auto INPUT_DOWN = 1 << 3;
  constexpr auto INPUT_A = 1 << 4;
  constexpr auto INPUT_B = 1 << 5;
  constexpr auto INPUT_C = 1 << 6;
  constexpr auto INPUT_LEFT_R = INPUT_LEFT << INPUT_RELSHIFT;
  constexpr auto INPUT_RIGHT_R = INPUT_RIGHT << INPUT_RELSHIFT;
  constexpr auto INPUT_UP_R = INPUT_UP << INPUT_RELSHIFT;
  constexpr auto INPUT_DOWN_R = INPUT_DOWN << INPUT_RELSHIFT;
  constexpr auto INPUT_A_R = INPUT_A << INPUT_RELSHIFT;
  constexpr auto INPUT_B_R = INPUT_B << INPUT_RELSHIFT;
  constexpr auto INPUT_C_R = INPUT_C << INPUT_RELSHIFT;

#ifndef COMPILER
  // Settable flags
  constexpr auto F_CTRL = 1 << 0;
  constexpr auto F_VISIBLE = 1 << 1;
  constexpr auto F_GRAVITY = 1 << 2;
  constexpr auto F_INVINCIBLE = 1 << 3;
  constexpr auto F_AIRDASH = 1 << 4;
  constexpr auto F_DOUBLEJUMP = 1 << 5;
  constexpr auto F_MIRROR = 1 << 6;

  // Un-settable flags
  constexpr auto F_ON_GROUND = 1 << 7;
  constexpr auto F_KNOCKDOWN = 1 << 8;
  constexpr auto F_BOUNCE = 1 << 9;
  constexpr auto F_OTG = 1 << 10;
  constexpr auto F_DEAD = 1 << 11;

  constexpr auto LEFT = 0;
  constexpr auto RIGHT = 1;

  constexpr auto INBUFF_SIZE = 32;
  constexpr auto NETBUFF_SIZE = 256;

  constexpr auto CANCEL_MAX = 32;

  constexpr auto DEFAULT_HP_MAX = 1000;
  constexpr auto SUPER_MAX = 1000;
  constexpr auto SUPER_DEFAULT = static_cast<int>(SUPER_MAX * 0.20);

  void initTransformSounds();
  void deinitTransformSounds();

  /// @brief Data structure containing information about an input
  struct InputBuff {
    uint32_t frame;
    uint16_t input;
  };

  /// @brief In-game interactable object
  /// @details Extended by Player
  class Projectile {
  public:
    Projectile();

    // Virtual
    virtual void think();
    virtual void interact(Projectile *other);
    virtual void advanceFrame();
    virtual void handleFrame(uint8_t command = 0);
    virtual void shootProjectile();
    virtual void draw() const;

    // Non-virtual
    void setState(int state);
    void setStandardState(unsigned int sstate);
    void playSound(int id) const;
    void say(int id) const;

    // Read from step memory
    int8_t readByte();
    int16_t readWord();
    int32_t readDword();
    float readFloat();
    std::string readString();

    bool isMirrored() const;

    bool inStandardState(unsigned int sstate) const;
    virtual bool isPlayer() const;

    int getPalette() const;
    void setPalette(int palette);
    const Fighter *getcFighter() const;
    void setFighter(Fighter *fighter);
    const glm::vec2 &getcrPos() const;
    void setPos(float x, float y);
    void setVel(float x, float y);
    void setDirection(char dir);
    uint32_t getFlags() const;
    void setFlags(uint32_t flags);
    unsigned int getState() const;
    unsigned int getDrawPriorityFrame() const;
    void setFlash(float flash);

  protected:
    int palette;
    Fighter *fighter;

    glm::vec2 pos;
    glm::vec2 vel;

    char dir;
    uint32_t flags;

    float scale;
    float idealScale;
    float spriteAlpha;
    float idealAlpha;

    // State control stuff
    int sprite;
    int wait;
    int timer;
    bool frameHit; // True if attack hit already this frame
    unsigned int state;
    unsigned int onhit;
    unsigned int step;

    char type;
    char movetype;

    // Attack stuff
    std::string spark;
    StepAttack attack;
    StepShoot shoot;
    StepBounce bounce;
    StepBounce bounceOther;
    bool knockdownOther;
    int stunOther;

    // Draw above priority frame
    unsigned int drawPriorityFrame;

    // Flash intensity
    float flash;
  };

  constexpr auto MAX_PROJECTILES = 128;

  /// @brief Active instance of in-game player
  /// @details Do not confuse with Fighter
  class Player : public Projectile {
  public:
    Player();
    ~Player();

    void reset(); // Resets back to beginning-of-round state

    void think() override final;
    void advanceFrame() override final;
    void handleFrame(uint8_t command = 0) override final;
    void shootProjectile() override final;
    void applyInput();
    void handleInput();
    void draw(bool shadow) const;
    void drawSpecial() const;

    void takeDamage(float damage);

    bool executeCommand(int cmd);
    // void executeFrame(StateFrame* sf, bool mine);

    void becomeIdle();

    bool isAttacking() const;
    bool isDashing() const;
    bool isIdle() const;
    bool isBeingHit() const;
    bool isInBlock() const;
    bool isKnockedBack() const;
    bool isKnockedProne() const;
    bool isKnocked() const;

    bool isStanding() const;
    bool isCrouching() const;
    bool isJumping() const;
    bool isBlocking() const;

    int getMaxHp() const;

    void setStateByInput(int state);
    void setStandardStateByInput(unsigned int sstate);

    bool isPlayer() const override final;

    static bool keycmp(uint16_t key1, uint16_t key2, bool generic);
    static uint16_t flipInput(uint16_t in);

    void setPlayerNumber(char playerNum);
    const audio::Speaker &getcrSpeaker() const;
    audio::Speaker &getrSpeaker();
    int getComboCounter() const;
    void setComboCounter(int comboCounter);
    uint16_t getFrameInput() const;
    void setFrameInput(uint16_t frameInput);
    void setFrameInputOR(uint16_t input);
    InputBuff *getNetBufferAt(int index);
    int getNetBufferCounter() const;
    void setNetBufferCounter(int netBuffCounter);
    float getJuggle() const;
    void setJuggle(float juggle);
    void setHitStun(int hitstun);
    void setPauseStun(int pausestun);
    void setCancelCount(int nCancels);
    int getHp() const;
    int getSuper() const;
    Projectile *getProjectileAt(int index);

  private:
    char playerNum;

    audio::Speaker speaker;

    // ubyte_t combo[256];
    int comboCounter;

    int nInputs;
    std::array<InputBuff, INBUFF_SIZE> inputs;

    // List of key inputs on this frame (gotten from either SFML or the network)
    uint16_t frameInput;
    uint16_t input; // Persistent input; what keys are held down at the moment
    std::array<InputBuff, NETBUFF_SIZE> netBuff;
    int netBuffCounter;

    // States and stuff
    int stateCombo;
    float juggle;
    int hitstun;
    int pausestun; // fancy hitstun, essentially
    int techstun;  // How long techs last

    // What can we cancel into?
    int nCancels;
    std::array<int, CANCEL_MAX> cancels;

    // Meters
    int hp;
    int super;

    // Special portrait
    mutable int special;
    bool ender;

    // Projectiles
    std::array<Projectile, MAX_PROJECTILES> projectiles;
    int projectileId;
  };
#endif // COMPILER
} // namespace game

#endif // DVD_PLAYER_H
