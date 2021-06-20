#include "../DvD/input.h"
#include "../DvD/fighter.h"
#include "../DvD/graphics.h"
#include "../DvD/sys.h"
#include "../renderer/gl_loader.h"

#include <SDL_keycode.h>
#include <glm/vec2.hpp>

extern game::Fighter fighter;
extern int frame;
extern int anim;

namespace input {
  static glm::ivec2 screenOffset;
  static glm::ivec2 screenSize;

  void resize(int w, int h) {
    if (h == 0) {
      h = 1;
    }

    int x = w;
    int y = h;

    float ratio1 = 1.0 * w / h;
    const float ratio2 = 1.0 * sys::WINDOW_WIDTH / sys::WINDOW_HEIGHT;
    if (ratio1 > ratio2) {
      x = h * ratio2;
    }
    else if (ratio1 < ratio2) {
      y = w / ratio2;
    }

    screenSize.x = x;
    screenSize.y = y;
    screenOffset.x = (w - x) / 2;
    screenOffset.y = (h - y) / 2;

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glViewport(screenOffset.x, screenOffset.y, x, y);
    glOrtho(0, sys::WINDOW_WIDTH, sys::WINDOW_HEIGHT, 0, 0, 1);
    glMatrixMode(GL_MODELVIEW);
  }

  static bool blackBG = true;

  static glm::ivec2 mousePos;
  static bool mouse1Down = false;
  static sprite::HitBox *selectBox = nullptr;
  static bool selectBoxAttack = false;
  static glm::ivec2 selectBoxOffset;

  static bool selectAll = false;

  static bool copyBoxes = false;
  static int copyFrame = -1;

  bool isBlackBG() { return blackBG; }

  sprite::HitBox *getSelectBox() { return selectBox; }
  bool isSelectBoxAttack() { return selectBoxAttack; }

  bool isSelectAll() { return selectAll; }

  void moveAll(int x, int y) {
    sprite::Sprite *sprite = fighter.getSpriteAt(frame);
    sprite::HitBoxGroup &hurtBoxes = sprite->getrDHurtBoxes();
    for (int i = 0; i < hurtBoxes.size; i++) {
      hurtBoxes.boxes[i].pos.x += x;
      hurtBoxes.boxes[i].pos.y += y;
    }
    sprite::HitBoxGroup &hitBoxes = sprite->getrAHitBoxes();
    for (int i = 0; i < hitBoxes.size; i++) {
      hitBoxes.boxes[i].pos.x += x;
      hitBoxes.boxes[i].pos.y += y;
    }
  }

  void keyPress(int key, bool press) {
    static int shift = 0;
    static int ctrl = 0;

    if (press) {
      sprite::Sprite *currentSprite = fighter.getSpriteAt(frame);
      sprite::HitBoxGroup &hurtBoxes = currentSprite->getrDHurtBoxes();
      sprite::HitBoxGroup &hitBoxes = currentSprite->getrAHitBoxes();
      switch (key) {
      // Invert screen colors
      case SDLK_q:
        if (blackBG) {
          graphics::setClearColor(255, 255, 255);
        }
        else {
          graphics::setClearColor(0, 0, 0);
        }
        blackBG = !blackBG;
        break;

      // COPY AND PASTE
      case SDLK_c:
        if (ctrl) {
          copyFrame = frame;
          if (selectAll) {
            copyBoxes = true;
          }
          else {
            copyBoxes = false;
          }
        }
        break;

      case SDLK_v:
        if (ctrl) {
          if (copyFrame == frame || copyFrame == -1) {
            return;
          }

          // Copy everything
          const sprite::Sprite *copySprite = fighter.getcSpriteAt(copyFrame);
          if (copyBoxes) {
            const sprite::HitBoxGroup &copyHurtBoxes =
                copySprite->getcrDHurtBoxes();
            const sprite::HitBoxGroup &copyHitBoxes =
                copySprite->getcrAHitBoxes();
            hurtBoxes.boxes.clear();
            hitBoxes.boxes.clear();
            hurtBoxes.init(copyHurtBoxes.size);
            hitBoxes.init(copyHitBoxes.size);

            for (int i = 0; i < hurtBoxes.size; i++) {
              hurtBoxes.boxes[i] = copyHurtBoxes.boxes[i];
            }
            for (int i = 0; i < hitBoxes.size; i++) {
              hitBoxes.boxes[i] = copyHitBoxes.boxes[i];
            }
          }
          else {
            currentSprite->setX(copySprite->getX());
            currentSprite->setY(copySprite->getY());
          }
        }
        break;

      // New hitbox
      case SDLK_s:
        if (ctrl) {
          fighter.saveSpr();
        }
        else {
          selectBox = hurtBoxes.newHitbox();
          selectBoxAttack = false;
        }
        break;

      // New attack hitbox
      case SDLK_a:
        if (ctrl) {
          selectBox = nullptr;
          selectAll = !selectAll;
        }
        else {
          selectBox = hitBoxes.newHitbox();
          selectBoxAttack = true;
        }
        break;

      // Delete hitbox
      case SDLK_x:
        if (selectBoxAttack) {
          hitBoxes.deleteHitbox(selectBox);
        }
        else {
          hurtBoxes.deleteHitbox(selectBox);
        }
        selectBox = nullptr;
        break;

      // Deselect
      case SDLK_d:
        selectBox = nullptr;
        break;

      // Select previous hitbox
      case SDLK_p: {
        selectAll = false;
        if (selectBox) {
          int i = 0;
          for (; i < hurtBoxes.size; i++)
            if (&hurtBoxes.boxes[i] == selectBox) {
              if (i == 0) {
                if (hitBoxes.size) {
                  selectBox = &hitBoxes.boxes.back();
                  selectBoxAttack = true;
                }
                else {
                  selectBox = &hurtBoxes.boxes.back();
                }
              }
              else {
                selectBox = &hurtBoxes.boxes[i - 1];
              }
              break;
            }
          if (i == hurtBoxes.size) {
            for (i = 0; i < hitBoxes.size; i++)
              if (&hitBoxes.boxes[i] == selectBox) {
                if (i == 0) {
                  if (hurtBoxes.size) {
                    selectBox = &hurtBoxes.boxes.back();
                    selectBoxAttack = false;
                  }
                  else {
                    selectBox = &hitBoxes.boxes.back();
                  }
                }
                else {
                  selectBox = &hitBoxes.boxes[i - 1];
                }
                break;
              }
          }
        }
        else {
          if (hurtBoxes.size) {
            selectBox = &hurtBoxes.boxes.front();
            selectBoxAttack = false;
          }
          else if (hitBoxes.size) {
            selectBox = &hitBoxes.boxes.front();
            selectBoxAttack = true;
          }
        }
      } break;

      // Select next hitbox
      case SDLK_n: {
        selectAll = false;
        if (selectBox) {
          int i = 0;
          for (; i < hurtBoxes.size; i++)
            if (&hurtBoxes.boxes[i] == selectBox) {
              if (i == hurtBoxes.size - 1) {
                if (hitBoxes.size) {
                  selectBox = &hitBoxes.boxes.front();
                  selectBoxAttack = true;
                }
                else {
                  selectBox = &hurtBoxes.boxes.front();
                }
              }
              else {
                selectBox = &hurtBoxes.boxes[i + 1];
              }
              break;
            }
          if (i == hurtBoxes.size) {
            for (i = 0; i < hitBoxes.size; i++)
              if (&hitBoxes.boxes[i] == selectBox) {
                if (i == hitBoxes.size - 1) {
                  if (hurtBoxes.size) {
                    selectBox = &hurtBoxes.boxes.front();
                    selectBoxAttack = false;
                  }
                  else {
                    selectBox = &hitBoxes.boxes.front();
                  }
                }
                else {
                  selectBox = &hitBoxes.boxes[i + 1];
                }
                break;
              }
          }
        }
        else {
          if (hurtBoxes.size) {
            selectBox = &hurtBoxes.boxes.front();
            selectBoxAttack = false;
          }
          else if (hitBoxes.size) {
            selectBox = &hitBoxes.boxes.front();
            selectBoxAttack = true;
          }
        }
      } break;

      case SDLK_h:
      case SDLK_LEFT:
        if (selectBox || selectAll) {
          if (shift) {
            if (ctrl) {
              selectBox->size.x -= 10;
            }
            else {
              selectBox->size.x--;
            }
            if (selectBox->size.x <= 0) {
              selectBox->size.x = 1;
            }
          }
          else {
            if (selectAll) {
              if (ctrl) {
                moveAll(-10, 0);
              }
              else {
                moveAll(-1, 0);
              }
            }
            else {
              if (ctrl) {
                selectBox->pos.x -= 10;
              }
              else {
                selectBox->pos.x--;
              }
            }
          }
        }
        else {
          if (ctrl) {
            currentSprite->setX(currentSprite->getX() + 10);
          }
          else {
            currentSprite->setX(currentSprite->getX() + 1);
          }
        }
        break;

      case SDLK_l:
      case SDLK_RIGHT:
        if (selectBox || selectAll) {
          if (shift) {
            if (ctrl) {
              selectBox->size.x += 10;
            }
            else {
              selectBox->size.x++;
            }
          }
          else {
            if (selectAll) {
              if (ctrl) {
                moveAll(10, 0);
              }
              else {
                moveAll(1, 0);
              }
            }
            else {
              if (ctrl) {
                selectBox->pos.x += 10;
              }
              else {
                selectBox->pos.x++;
              }
            }
          }
        }
        else {
          if (ctrl) {
            currentSprite->setX(currentSprite->getX() - 10);
          }
          else {
            currentSprite->setX(currentSprite->getX() - 1);
          }
        }
        break;

      case SDLK_k:
      case SDLK_UP:
        if (selectBox || selectAll) {
          if (shift) {
            if (ctrl) {
              selectBox->size.y += 10;
            }
            else {
              selectBox->size.y++;
            }
          }
          else {
            if (selectAll) {
              if (ctrl) {
                moveAll(0, 10);
              }
              else {
                moveAll(0, 1);
              }
            }
            else {
              if (ctrl) {
                selectBox->pos.y += 10;
              }
              else {
                selectBox->pos.y++;
              }
            }
          }
        }
        else {
          if (ctrl) {
            currentSprite->setY(currentSprite->getY() - 10);
          }
          else {
            currentSprite->setY(currentSprite->getY() - 1);
          }
        }
        break;

      case SDLK_j:
      case SDLK_DOWN:
        if (selectBox || selectAll) {
          if (shift) {
            if (ctrl) {
              selectBox->size.y -= 10;
            }
            else {
              selectBox->size.y--;
            }
            if (selectBox->size.y <= 0) {
              selectBox->size.y = 1;
            }
          }
          else {
            if (selectAll) {
              if (ctrl) {
                moveAll(0, -10);
              }
              else {
                moveAll(0, -1);
              }
            }
            else {
              if (ctrl) {
                selectBox->pos.y -= 10;
              }
              else {
                selectBox->pos.y--;
              }
            }
          }
        }
        else {
          if (ctrl) {
            currentSprite->setY(currentSprite->getY() + 10);
          }
          else {
            currentSprite->setY(currentSprite->getY() + 1);
          }
        }
        break;

      case SDLK_o: {
        // case SDLK_PageUp:
        selectBox = nullptr;
        selectAll = false;
        int nSprites = fighter.getSpriteCount();
        if (ctrl) {
          if (frame <= nSprites - 1 - 10) {
            frame += 10;
          }
          else {
            frame = nSprites - 1;
          }
        }
        else if (frame < nSprites - 1) {
          frame++;
        }
        break;
      }

      case SDLK_i:
        // case SDLK_PageDown:
        selectBox = nullptr;
        selectAll = false;
        if (ctrl) {
          if (frame >= 10) {
            frame -= 10;
          }
          else {
            frame = 0;
          }
        }
        else if (frame) {
          frame--;
        }
        break;

      case SDLK_LSHIFT:
        shift |= 1;
        break;

      case SDLK_RSHIFT:
        shift |= 2;
        break;

      case SDLK_LCTRL:
        ctrl |= 1;
        break;

      case SDLK_RCTRL:
        ctrl |= 2;
        break;
      }
    }
    else {
      switch (key) {
      case SDLK_LSHIFT:
        shift &= ~1;
        break;

      case SDLK_RSHIFT:
        shift &= ~2;
        break;

      case SDLK_LCTRL:
        ctrl &= ~1;
        break;

      case SDLK_RCTRL:
        ctrl &= ~2;
        break;
      }
    }
  }

  void mouseMove(int x, int y) {
    // Calculates mouse pos based on size of screen
    float ratio = 1.0f;
    if (screenOffset.x) {
      ratio = (float)sys::WINDOW_HEIGHT / screenSize.y;
    }
    else if (screenOffset.y) {
      ratio = (float)sys::WINDOW_WIDTH / screenSize.x;
    }
    mousePos.x = (x - screenOffset.x) * ratio;
    mousePos.y = (y - screenOffset.y) * ratio;
    if (mousePos.x < 0) {
      mousePos.x = 0;
    }
    if (mousePos.x >= sys::WINDOW_WIDTH) {
      mousePos.x = sys::WINDOW_WIDTH - 1;
    }
    if (mousePos.y < 0) {
      mousePos.y = 0;
    }
    if (mousePos.y >= sys::WINDOW_HEIGHT) {
      mousePos.y = sys::WINDOW_HEIGHT - 1;
    }

    // Do stuff if a hitbox is selected
    if (mouse1Down && selectBox) {
      selectBox->pos.x = (mousePos.x + selectBoxOffset.x) / 2;
      selectBox->pos.y = (sys::FLIP(mousePos.y) + selectBoxOffset.y) / 2;
    }
  }

  void mousePress(int key, bool press) {
    if (press)
      switch (key) {
      case 0: {
        sprite::Sprite *sprite = fighter.getSpriteAt(frame);
        sprite::HitBoxGroup &hurtBoxes = sprite->getrDHurtBoxes();
        sprite::HitBoxGroup &hitBoxes = sprite->getrAHitBoxes();
        mouse1Down = true;
        int i = 0;
        for (; i < hurtBoxes.size; i++)
          if (hurtBoxes.boxes[i].collidePoint(mousePos.x -
                                                  sys::WINDOW_WIDTH / 2,
                                              mousePos.y + sys::EDIT_OFFSET)) {
            selectAll = false; // TODO fix this
            selectBox = &hurtBoxes.boxes[i];
            selectBoxAttack = false;
            selectBoxOffset.x = selectBox->pos.x - mousePos.x;
            selectBoxOffset.y = selectBox->pos.y - sys::FLIP(mousePos.y);
            break;
          }
        if (i == hurtBoxes.size) {
          selectBox = nullptr;
        }
        if (!selectBox) {
          for (i = 0; i < hitBoxes.size; i++)
            if (hitBoxes.boxes[i].collidePoint(mousePos.x -
                                                   sys::WINDOW_WIDTH / 2,
                                               mousePos.y + sys::EDIT_OFFSET)) {
              selectAll = false; // TODO fix this
              selectBox = &hitBoxes.boxes[i];
              selectBoxAttack = true;
              selectBoxOffset.x = selectBox->pos.x - mousePos.x;
              selectBoxOffset.y = selectBox->pos.y - sys::FLIP(mousePos.y);
              break;
            }
          if (i == hitBoxes.size) {
            selectBox = nullptr;
            selectAll = false;
          }
        }
      } break;
      case 1:
        break;
      }
    else
      switch (key) {
      case 0:
        mouse1Down = false;
        break;
      }
  }
} // namespace input
