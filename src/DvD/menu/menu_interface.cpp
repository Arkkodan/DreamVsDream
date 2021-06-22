#include "menu_interface.h"

menu::IMenuElement::IMenuElement() {}
menu::IMenuElement::~IMenuElement() {}

void menu::IMenuElement::think() {}
void menu::IMenuElement::doInput(uint16_t input) {}
void menu::IMenuElement::doInputP(uint16_t input, uint8_t playerIndex) {}
void menu::IMenuElement::reset() {}
void menu::IMenuElement::draw() const {}

bool menu::IMenuElement::isActive() const { return active; }
void menu::IMenuElement::setActive(bool active) { this->active = active; }
bool menu::IMenuElement::isEnabled() const { return enabled; }
void menu::IMenuElement::setEnabled(bool enabled) { this->enabled = enabled; }

menu::ITextMenuElement::ITextMenuElement() {}
menu::ITextMenuElement::~ITextMenuElement() {}

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

void menu::ITextMenuElement::reset() {
  if (active) {
    x = restX + activeShiftX;
  }
  else {
    x = restX;
  }
}

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

menu::IMenu::~IMenu() {}
