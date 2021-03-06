#ifndef DVD_EFFECT_H
#define DVD_EFFECT_H

#include "image.h"
#include "player.h"

#include <string>
#include <vector>

namespace effect {
  /// @brief Data entry containing animation of an effect
  class EffectAnimation {
  public:
    EffectAnimation();
    EffectAnimation(EffectAnimation &&other) noexcept;
    EffectAnimation &operator=(EffectAnimation &&other) noexcept;
    ~EffectAnimation();

    EffectAnimation(std::string name);

    // Do not copy anims
    EffectAnimation(const EffectAnimation &other) = delete;
    EffectAnimation &operator=(EffectAnimation &other) = delete;

    std::string getName() const;
    Image *getFrame(int frame);
    int getNumFrames() const;

  private:
    std::string name;
    std::vector<Image> frames;
    int nFrames;
  };

  /// @brief In-game effect object
  class Effect {
  public:
    Effect();
    Effect(const std::string &name, int x, int y, bool moveWithCamera,
           bool mirror, int speed, int nLoops, game::Projectile *parent);
    ~Effect();

    unsigned int getCreationFrame() const;

    bool exists() const;
    void draw() const;

  private:
    EffectAnimation *anim;
    int speed;
    unsigned int frameStart;
    unsigned int frameEnd;

    int x, y;
    bool moveWithCamera;
    bool mirror;

    game::Projectile *parent;
  };

  /// @brief Create EffectAnimation objects
  void init();
  /// @brief Free EffectAnimation objects
  void deinit();

  /// @brief Create a new in-game effect
  void newEffect(const std::string &name, int x, int y, bool moveWithCamera,
                 bool mirror, int speed, int nLoops, game::Projectile *parent);
  /// @brief Draw all in-game effects
  void draw();
} // namespace effect

#endif // DVD_EFFECT_H
