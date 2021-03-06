#include "sprite.h"

#include "graphics.h"
#include "sys.h"
#ifdef SPRTOOL
#include "input.h"
#include "shader_renderer/texture2D_renderer.h"
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
    renderer::PrimitiveRenderer::setPosRect(
        static_cast<float>(x), static_cast<float>(x + w), static_cast<float>(y),
        static_cast<float>(y - h));
    renderer::PrimitiveRenderer::draw();
  }

  void HitBox::draw(int _x, int _y, bool attack, bool selected) const {
    _x += sys::WINDOW_WIDTH / 2;
    _y = sys::FLIP(_y);

    if (attack) {
      renderer::PrimitiveRenderer::setColor({1.0f, 0.0f, 0.0f, 1.0f});
    }
    else {
      renderer::PrimitiveRenderer::setColor({0.5f, 0.5f, 1.0f, 1.0f});
    }

    drwbx(_x + pos.x, _y - pos.y, size.x, 1);              // Bottom
    drwbx(_x + pos.x, _y - pos.y - size.y + 1, size.x, 1); // Top
    drwbx(_x + pos.x, _y - pos.y, 1, size.y);              // Left
    drwbx(_x + pos.x + size.x - 1, _y - pos.y, 1, size.y); // Right
    if (selected) {
      if (attack) {
        renderer::PrimitiveRenderer::setColor({1.0f, 0.0f, 0.0f, 0.5f});
      }
      else {
        renderer::PrimitiveRenderer::setColor({0.5f, 0.5f, 1.0f, 0.5f});
      }
      drwbx(_x + pos.x, _y - pos.y, size.x, size.y);
    }

    renderer::ShaderProgram::unuse();
  }

  bool HitBox::collideOther(HitBox *other, glm::ivec2 *colpos,
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
    adj.size.x = static_cast<int>(size.x * scale);
    adj.size.y = static_cast<int>(size.y * scale);

    if (m) {
      adj.pos.x = static_cast<int>(_x + pos.x * -scale + size.x * -scale);
    }
    else {
      adj.pos.x = static_cast<int>(_x + pos.x * scale);
    }
    adj.pos.y = static_cast<int>(_y + pos.y * scale);

    return adj;
  }

  int Sprite::collide(int x1, int y1, int x2, int y2, bool m1, bool m2,
                      float scale1, float scale2, const Sprite *other,
                      glm::ivec2 *colpos, bool allowOutOfBounds) const {
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
      for (int j = 0; j < other->hurtBoxes.size; j++) {
        HitBox you = other->hurtBoxes.boxes[j].adjust(x2, y2, m2, scale2);
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
      _x -= static_cast<int>((img.getW() - x) * scale);
    }
    else {
      _x -= static_cast<int>(x * scale);
    }
#else
    AtlasSprite _atlas_sprite = atlas->getSprite(atlas_sprite);

    if (mirror) {
      _x -= static_cast<int>((_atlas_sprite.w - x) * scale);
    }
    else {
      _x -= static_cast<int>(x * scale);
    }
#endif
    _y -= static_cast<int>(y * scale);

    graphics::setScale(scale);
#ifdef GAME
    atlas->drawSprite(atlas_sprite, _x, _y, mirror);
#else
    img.drawSprite<renderer::Texture2DRenderer>(_x, _y, mirror);
    bool selectAll = input::isSelectAll();
    const auto *selectBox = input::getSelectBox();
    bool selectBoxAttack = input::isSelectBoxAttack();
    for (int i = 0; i < hurtBoxes.size; i++) {
      hurtBoxes.boxes[i].draw(
          x2, y2, false,
          selectAll ||
              ((&hurtBoxes.boxes[i] == selectBox) && !selectBoxAttack));
    }
    for (int i = 0; i < aHitBoxes.size; i++) {
      aHitBoxes.boxes[i].draw(
          x2, y2, true,
          selectAll || ((&aHitBoxes.boxes[i] == selectBox) && selectBoxAttack));
    }
#endif
  }

#ifndef SPRTOOL
  void Sprite::drawShadow(int _x, bool mirror, float scale) const {
    if (mirror) {
      _x -= static_cast<int>(atlas->getSprite(atlas_sprite).w * scale -
                             x * scale);
    }
    else {
      _x -= static_cast<int>(x * scale);
    }
    renderer::FighterRenderer::setColor({0.0f, 0.0f, 0.0f});
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
        hurtBoxes(), aHitBoxes() {
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
        hurtBoxes(other.hurtBoxes), aHitBoxes(other.aHitBoxes) {
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
    hurtBoxes = std::move(other.hurtBoxes);
    aHitBoxes = std::move(other.aHitBoxes);

    return *this;
  }

  int Sprite::getX() const { return x; }
  void Sprite::setX(int x) { this->x = x; }
  int Sprite::getY() const { return y; }
  void Sprite::setY(int y) { this->y = y; }

#ifdef SPRTOOL
  Image *Sprite::getImage() { return &img; }
#endif // SPRTOOL

#ifdef GAME
  const Atlas *Sprite::getcAtlas() const { return atlas; }
  void Sprite::setAtlas(Atlas *atlas) { this->atlas = atlas; }
  int Sprite::getAtlasSprite() const { return atlas_sprite; }
  void Sprite::setAtlasSprite(int atlas_sprite) {
    this->atlas_sprite = atlas_sprite;
  }
#else // !GAME
  std::string Sprite::getName() const { return name; }
  void Sprite::setName(const std::string &name) { this->name = name; }
#endif

  const HitBoxGroup &Sprite::getcrDHurtBoxes() const { return hurtBoxes; }
  HitBoxGroup &Sprite::getrDHurtBoxes() { return hurtBoxes; }
  const HitBoxGroup &Sprite::getcrAHitBoxes() const { return aHitBoxes; }
  HitBoxGroup &Sprite::getrAHitBoxes() { return aHitBoxes; }

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

    boxes[size].size = glm::ivec2(15, 15);
    return &boxes[size++];
  }

  void HitBoxGroup::deleteHitbox(HitBox *box) {
    if (!box || !size) {
      return;
    }

    unsigned int i;
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
