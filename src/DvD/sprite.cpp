#include "sprite.h"

#include "graphics.h"
#include "sys.h"
#ifdef SPRTOOL
#include "input.h"
#endif // SPRTOOL
#ifndef COMPILER
#include "shader_renderer/fighter_renderer.h"
#include "shader_renderer/primitive_renderer.h"
#endif // COMPILER

#include <algorithm>

namespace sprite {

  HitBox::HitBox() {}
  HitBox::HitBox(int x, int y, int w, int h) {
    pos.x = x;
    pos.y = y;
    size.x = w;
    size.y = h;
  }

#ifndef COMPILER

  bool HitBox::collidePoint(int pX, int pY) const {
    pY = sys::FLIP(pY);

    if (pos.x <= pX && pX <= pos.x + size.x && pos.y <= pY &&
        pY <= pos.y + size.y) {
      return true;
    }
    return false;
  }

  void drwbx(int x, int y, int w, int h) {
    renderer::PrimitiveRenderer::setPosRect(x, x + w, y, y - h);
    renderer::PrimitiveRenderer::draw();
  }

  void HitBox::draw(int _x, int _y, bool attack, bool selected) const {
    _x += sys::WINDOW_WIDTH / 2;
    _y = sys::FLIP(_y);

    if (attack) {
      renderer::PrimitiveRenderer::setColor(1.0f, 0.0f, 0.0f, 1.0f);
    }
    else {
      renderer::PrimitiveRenderer::setColor(0.5f, 0.5f, 1.0f, 1.0f);
    }

    drwbx(_x + pos.x, _y - pos.y, size.x, 1);              // Bottom
    drwbx(_x + pos.x, _y - pos.y - size.y + 1, size.x, 1); // Top
    drwbx(_x + pos.x, _y - pos.y, 1, size.y);              // Left
    drwbx(_x + pos.x + size.x - 1, _y - pos.y, 1, size.y); // Right
    if (selected) {
      if (attack) {
        renderer::PrimitiveRenderer::setColor(1.0f, 0.0f, 0.0f, 0.5f);
      }
      else {
        renderer::PrimitiveRenderer::setColor(0.5f, 0.5f, 1.0f, 0.5f);
      }
      drwbx(_x + pos.x, _y - pos.y, size.x, size.y);
    }

    renderer::ShaderProgram::unuse();
  }

  bool HitBox::collideOther(HitBox *other, util::Vector *colpos,
                            bool allowOutOfBounds) const {
    if (pos.x + size.x < other->pos.x) {
      return false;
    }
    if (pos.x > other->pos.x + other->size.x) {
      return false;
    }
    if (pos.y + size.y < other->pos.y) {
      return false;
    }
    if (pos.y > other->pos.y + other->size.y) {
      return false;
    }

    // Calculate colpos
    if (colpos) {
      // colpos->x = (pos.x + other->pos.x) / 2 + (size.x + other->size.x) / 2;
      // colpos->y = (pos.y + other->pos.y) / 2 + (size.y + other->size.y) / 2;
      if (pos.x < other->pos.x) {
        colpos->x = other->pos.x + (pos.x + size.x - other->pos.x) / 2;
      }
      else {
        colpos->x = pos.x + (other->pos.x + other->size.x - pos.x) / 2;
      }
      if (pos.y < other->pos.y) {
        colpos->y = other->pos.y + (pos.y + size.y - other->pos.y) / 2;
      }
      else {
        colpos->y = pos.y + (other->pos.y + other->size.y - pos.y) / 2;
      }

      if (!allowOutOfBounds) {
        if (colpos->x < other->pos.x) {
          colpos->x = other->pos.x;
        }
        else if (colpos->x > other->pos.x + other->size.x - 1) {
          colpos->x = other->pos.x + other->size.x - 1;
        }
        if (colpos->y < other->pos.y) {
          colpos->y = other->pos.y;
        }
        else if (colpos->y > pos.y + other->size.y - 1) {
          colpos->y = other->pos.y + other->size.y - 1;
        }
      }
    }
    return true;
  }

  HitBox HitBox::adjust(int _x, int _y, bool m, float scale) const {
    HitBox adj;
    adj.size.x = size.x * scale;
    adj.size.y = size.y * scale;

    if (m) {
      adj.pos.x = _x + pos.x * -scale + size.x * -scale;
    }
    else {
      adj.pos.x = _x + pos.x * scale;
    }
    adj.pos.y = _y + pos.y * scale;

    return adj;
  }

  int Sprite::collide(int x1, int y1, int x2, int y2, bool m1, bool m2,
                      float scale1, float scale2, Sprite *other,
                      util::Vector *colpos, bool allowOutOfBounds) const {
    // Check for attack hitbox collision with other sprite
    for (int i = 0; i < aHitBoxes.size; i++) {
      HitBox me = aHitBoxes.boxes[i].adjust(x1, y1, m1, scale1);

      // First, enemy attack hitboxes
      for (int j = 0; j < other->aHitBoxes.size; j++) {
        HitBox you = other->aHitBoxes.boxes[j].adjust(x2, y2, m2, scale2);
        if (me.collideOther(&you, colpos, allowOutOfBounds)) {
          return HIT_ATTACK;
        }
      }
      // Now, normal enemy hitboxes
      for (int j = 0; j < other->hitBoxes.size; j++) {
        HitBox you = other->hitBoxes.boxes[j].adjust(x2, y2, m2, scale2);
        if (me.collideOther(&you, colpos, allowOutOfBounds)) {
          return HIT_HIT;
        }
      }
    }
    return HIT_NOT;
  }

  void Sprite::draw(int _x, int _y, bool mirror, float scale) const {
#ifdef SPRTOOL
    int x2 = _x;
    int y2 = _y;

    if (mirror) {
      _x -= (img.w - x) * scale;
    }
    else {
      _x -= x * scale;
    }
#else
    AtlasSprite _atlas_sprite = atlas->getSprite(atlas_sprite);

    if (mirror) {
      _x -= (_atlas_sprite.w - x) * scale;
    }
    else {
      _x -= x * scale;
    }
#endif
    _y -= y * scale;

    graphics::setScale(scale);
#ifdef GAME
    atlas->drawSprite(atlas_sprite, _x, _y, mirror);
#else
    img.drawSprite<renderer::FighterRenderer>(_x, _y, mirror);
    for (int i = 0; i < hitBoxes.size; i++) {
      hitBoxes.boxes[i].draw(x2, y2, false,
                             input::selectAll ||
                                 ((&hitBoxes.boxes[i] == input::selectBox) &&
                                  !input::selectBoxAttack));
    }
    for (int i = 0; i < aHitBoxes.size; i++) {
      aHitBoxes.boxes[i].draw(x2, y2, true,
                              input::selectAll ||
                                  ((&aHitBoxes.boxes[i] == input::selectBox) &&
                                   input::selectBoxAttack));
    }
#endif
  }

#ifndef SPRTOOL
  void Sprite::drawShadow(int _x, bool mirror, float scale) const {
    if (mirror) {
      _x -= atlas->getSprite(atlas_sprite).w * scale - x * scale;
    }
    else {
      _x -= x * scale;
    }
    renderer::FighterRenderer::setColor(0.0f, 0.0f, 0.0f);
    renderer::FighterRenderer::setAlpha(0.5f);
    graphics::setScale(scale, 0.2f * scale);
    atlas->drawSprite(atlas_sprite, _x, 0, mirror);
  }
#endif
#endif

  Sprite::Sprite()
      : x(0), y(0)
#ifdef SPRTOOL
        ,
        img()
#endif
#ifdef GAME
        ,
        atlas(nullptr), atlas_sprite(0)
#else
        ,
        name()
#endif
        ,
        hitBoxes(), aHitBoxes() {
  }

  Sprite::~Sprite() {}

  Sprite::Sprite(Sprite &&other) noexcept
      : x(other.x), y(other.y)
#ifdef SPRTOOL
        ,
        img(std::move(other.img))
#endif
#ifdef GAME
        ,
        atlas(other.atlas), atlas_sprite(other.atlas_sprite)
#else
        ,
        name(std::move(other.name))
#endif
        ,
        hitBoxes(other.hitBoxes), aHitBoxes(other.aHitBoxes) {
#ifdef GAME
    other.atlas = nullptr;
#endif
  }
  Sprite &Sprite::operator=(Sprite &&other) noexcept {
    x = other.x;
    y = other.y;
#ifdef SPRTOOL
    img = std::move(other.img);
#endif
#ifdef GAME
    atlas = other.atlas;
    other.atlas = nullptr;
    atlas_sprite = other.atlas_sprite;
#else
    name = std::move(other.name);
#endif
    hitBoxes = std::move(other.hitBoxes);
    aHitBoxes = std::move(other.aHitBoxes);

    return *this;
  }

  HitBoxGroup::HitBoxGroup() {
    size = 0;
    boxes.clear();
  }

  HitBoxGroup::~HitBoxGroup() {}

  void HitBoxGroup::init(int size) {
    this->size = size;
    if (size) {
      boxes.resize(size);
    }
    else {
      boxes.clear();
    }
  }

#ifdef SPRTOOL
  HitBox *HitBoxGroup::newHitbox() {
    boxes.resize(size + 1);

    boxes[size].size = util::Vector(15, 15);
    return &boxes[size++];
  }

  void HitBoxGroup::deleteHitbox(HitBox *box) {
    if (!box || !size) {
      return;
    }

    int i;
    for (i = 0; i < boxes.size(); i++) {
      if (&boxes[i] == box) {
        break;
      }
    }

    boxes.erase(std::remove_if(boxes.begin(), boxes.end(),
                               [box](HitBox &h) { return &h == box; }),
                boxes.end());
    size = boxes.size();
  }
#endif
} // namespace sprite
