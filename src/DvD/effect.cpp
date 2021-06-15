#include "effect.h"

#include "../util/fileIO.h"
#include "error.h"
#include "graphics.h"
#include "resource_manager.h"
#include "shader_renderer/texture2D_renderer.h"
#include "sys.h"

#include <algorithm>
#include <array>

namespace effect {
  constexpr auto EFFECT_MAX = 256;

  // VARIABLES
  int nEffectAnims = 0;
  static std::vector<EffectAnimation *> effectAnims;

  std::array<Effect, EFFECT_MAX> effects;

  // EFFECT ANIMATION
  EffectAnimation::EffectAnimation() {
    name = "";
    nFrames = 0;
  }

  EffectAnimation::EffectAnimation(EffectAnimation &&other) noexcept {
    name = std::move(other.name);
    nFrames = other.nFrames;
    frames = std::move(other.frames);
  }

  EffectAnimation &
  EffectAnimation::operator=(EffectAnimation &&other) noexcept {
    nFrames = other.nFrames;
    name.swap(other.name);
    frames.swap(other.frames);
    return *this;
  }

  EffectAnimation::EffectAnimation(std::string _name) : name(std::move(_name)) {
    // Get the list of files
    std::vector<std::string> files =
        util::listDirectory(util::getPath("effects/" + name), true);

    nFrames = 0;

    if (files.empty()) {
      return;
    }

    // See if all of our images exist first, and count them.
    // Thumbs.db, .DS_Store, etc could screw the list up.
    for (std::vector<std::string>::size_type i = 0, last = -1; i < files.size();
         i++) {
      if (std::find(files.begin(), files.end(),
                    util::toString(i + 1) + ".png") != files.end()) {
        if (last != i - 1) {
          error::error("Missing frames in effect animation \"" + name + "\".");
          return;
        }
        nFrames++;
        last++;
      }
    }

    if (!nFrames) {
      return;
    }

    // Allocate array and try to populate
    frames.resize(nFrames);

    for (int i = 0; i < nFrames; i++) {
      frames[i].createFromFile("effects/" + name + "/" + util::toString(i + 1) +
                               ".png");
    }
  }

  EffectAnimation::~EffectAnimation() {}

  std::string EffectAnimation::getName() const { return name; }

  Image *EffectAnimation::getFrame(int frame) {
    if (nFrames == 0)
      return nullptr;

    return &frames[frame % nFrames];
  }

  int EffectAnimation::getNumFrames() const { return nFrames; }

  // EFFECT
  Effect::Effect()
      : anim(nullptr), moveWithCamera(false), mirror(false), speed(0),
        parent(nullptr) {
    speed = frameStart = frameEnd = x = y = 0;
  }

  Effect::Effect(const std::string &name, int x, int y, bool moveWithCamera,
                 bool mirror, int speed, int nLoops, game::Projectile *_parent)
      : moveWithCamera(moveWithCamera), mirror(mirror), speed(speed),
        parent(_parent) {
    anim = nullptr;

    // Look up the animation
    int i = 0;
    for (; i < nEffectAnims; i++) {
      if (name == effectAnims[i]->getName()) {
        anim = effectAnims[i];
        break;
      }
    }

    // We failed to find the animation
    if (i == nEffectAnims) {
      error::error("Could not find the effect animation \"" + name + "\".");
      this->x = this->y = this->frameEnd = this->frameStart = 0;
    }
    else {
      // Create object
      this->x = x;
      this->y = y;
      this->moveWithCamera = moveWithCamera;
      this->mirror = mirror;
      this->speed = speed;

      // Calculate the start and end frames
      this->frameStart = sys::frame;
      this->frameEnd = this->frameStart +
                       (unsigned int)(anim->getNumFrames() * nLoops * speed);
    }
  }

  Effect::~Effect() {}

  unsigned int Effect::getCreationFrame() const { return frameStart; }

  bool Effect::exists() const { return frameEnd > sys::frame; }

  void Effect::draw() const {
    if (!exists())
      return;

    Image *frame = anim->getFrame((sys::frame - frameStart) / speed);

    int x1 = x - frame->w / 2;
    int y1 = y - frame->h / 2;

    if (parent) {
      x1 += parent->pos.x;
      y1 += parent->pos.y;
    }

    graphics::setRender(Image::Render::ADDITIVE);
    if (moveWithCamera)
      frame->drawSprite<renderer::Texture2DRenderer>(x1, y1, mirror);
    else
      frame->draw<renderer::Texture2DRenderer>(x1, y1, mirror);
  }

  // MISC FUNCS
  void init() {
    resource_manager::loadFromManifest<EffectAnimation>();
    effectAnims = resource_manager::getFromManifest<EffectAnimation>();
    nEffectAnims = effectAnims.size();
  }

  void deinit() {}

  void newEffect(const std::string &name, int x, int y, bool moveWithCamera,
                 bool mirror, int speed, int nLoops, game::Projectile *parent) {
    for (int i = 0; i < EFFECT_MAX; i++) {
      if (!effects[i].exists()) {
        effects[i] =
            Effect(name, x, y, moveWithCamera, mirror, speed, nLoops, parent);
        break;
      }
    }
  }

  void draw() {
    // Be sure to draw all the effects in the correct order
    bool drawn[EFFECT_MAX] = {false};

    for (;;) {
      int iEarliest = -1;
      unsigned int earliest = UINT_MAX;
      for (int i = 0; i < EFFECT_MAX; i++) {
        if (!drawn[i] && effects[i].exists() &&
            effects[i].getCreationFrame() < earliest) {
          iEarliest = i;
          earliest = effects[i].getCreationFrame();
        }
      }
      if (iEarliest == -1)
        break;
      drawn[iEarliest] = true;

      effects[iEarliest].draw();
    }
  }
} // namespace effect
