#include "menu_interface.h"

bool menu::IMenuElement::isActive() const { return active; }
void menu::IMenuElement::setActive(bool active) { this->active = active; }
bool menu::IMenuElement::isEnabled() const { return enabled; }
void menu::IMenuElement::setEnabled(bool enabled) { this->enabled = enabled; }

void menu::ITextMenuElement::think() {
  if (active) {
    if (x != restX + activeShiftX) {
      x += (restX + activeShiftX - x) / 2;
      if (abs(restX + activeShiftX - x) <= 1) {
        x = restX + activeShiftX;
      }
    }
  }
  else {
    if (x != restX) {
      x += (restX - x) / 2;
      if (abs(restX - x) <= 1) {
        x = restX;
      }
    }
  }
}

void menu::ITextMenuElement::reset() { x = restX; }

void menu::ITextMenuElement::setFont(Font *font) {
  if (font && font->exists()) {
    this->font = font;
  }
}
void menu::ITextMenuElement::setPos(int restX, int y, int activeShiftX) {
  this->restX = restX;
  this->y = y;
  this->activeShiftX = activeShiftX;
}
void menu::ITextMenuElement::setColorActive(uint8_t aR, uint8_t aG,
                                            uint8_t aB) {
  this->aR = aR;
  this->aG = aG;
  this->aB = aB;
}
void menu::ITextMenuElement::setColorInctive(uint8_t iR, uint8_t iG,
                                             uint8_t iB) {
  this->iR = iR;
  this->iG = iG;
  this->iB = iB;
}
