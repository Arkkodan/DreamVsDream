#include "button.h"

menu::ButtonA::ButtonA() : action(), sndSelect(nullptr), sndInvalid(nullptr) {
  active = enabled = false;
}
menu::ButtonA::~ButtonA() {}

void menu::ButtonA::think() {}

void menu::ButtonA::doInput(uint16_t input) {
  if (input & game::INPUT_A) {
    if (enabled) {
      action();
      if (sndSelect) {
        sndSelect->play();
      }
    }
    else {
      if (sndInvalid) {
        sndInvalid->play();
      }
    }
  }
}

void menu::ButtonA::doInput(uint16_t input, uint8_t playerIndex) {
  doInput(input);
}

void menu::ButtonA::reset() {}

void menu::ButtonA::draw() const {}

void menu::ButtonA::setAction(std::function<void(void)> action) {
  this->action = action;
  enabled = true;
}

void menu::ButtonA::setSelectSound(const audio::Sound *sndSelect) {
  this->sndSelect = sndSelect;
}
void menu::ButtonA::setInvalidSound(const audio::Sound *sndInvalid) {
  this->sndInvalid = sndInvalid;
}

menu::TextButtonA::TextButtonA() : text("") {
  font = nullptr;
  x = y = restX = activeShiftX = 0;
  aR = aG = aB = 255;
  iR = iG = iB = 127;
}

menu::TextButtonA::~TextButtonA() {}

void menu::TextButtonA::think() {
  ButtonA::think();
  ITextMenuElement::think();
}

void menu::TextButtonA::reset() {
  ButtonA::think();
  ITextMenuElement::reset();
}

void menu::TextButtonA::draw() const {
  if (font) {
    uint8_t r, g, b;

    if (active) {
      r = aR;
      g = aG;
      b = aB;
    }
    else {
      r = iR;
      g = iG;
      b = iB;
    }
    if (!enabled) {
      r /= 2;
      g /= 2;
      b /= 2;
    }
    font->drawText(x, y, text, r, g, b);
  }
}

void menu::TextButtonA::setText(const std::string &text) { this->text = text; }
