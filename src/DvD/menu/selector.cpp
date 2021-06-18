#include "selector.h"

#include <stdexcept>

template <typename T>
menu::SelectorLR<T>::SelectorLR() : values(), index(0), wrapIndex(false) {
  active = enabled = false;
}

template <typename T> void menu::SelectorLR<T>::think() {}

template <typename T> void menu::SelectorLR<T>::doInput(uint16_t input) {
  int size = values.size();
  if (!enabled || size == 0) {
    return;
  }

  int changeIndex = ((input & game::INPUT_RIGHT) ? 1 : 0) -
                    ((input & game::INPUT_LEFT) ? 1 : 0);

  index += changeIndex;
  if (wrapIndex) {
    while (index < 0) {
      index += size;
    }
    while (index >= size) {
      index -= size;
    }
  }
  else {
    if (index < 0) {
      index = 0;
    }
    else if (index >= size) {
      index = size - 1;
    }
  }
}

template <typename T>
void menu::SelectorLR<T>::doInput(uint16_t input, uint8_t playerIndex) {
  doInput(input);
}

template <typename T> void menu::SelectorLR<T>::reset() {}

template <typename T> void menu::SelectorLR<T>::draw() const {}

template <typename T>
void menu::SelectorLR<T>::setValues(const std::vector<const T> &values) {
  this->values = values;
}
template <typename T> void menu::SelectorLR<T>::setIndex(int index) {
  if (index < 0 || index >= values.size()) {
    throw std::out_of_range("invalid Selector index: " + std::to_string(index));
  }
  this->index = index;
}
template <typename T> T menu::SelectorLR<T>::getValue() const {
  return values[index];
}

template <typename T> bool menu::SelectorLR<T>::isActive() const {
  return active;
}
template <typename T> void menu::SelectorLR<T>::setActive(bool active) {
  this->active = active;
}

template <typename T> bool menu::SelectorLR<T>::isEnabled() const {
  return enabled;
}
template <typename T> void menu::SelectorLR<T>::setEnabled(bool enabled) {
  this->enabled = enabled;
}

template <typename T> menu::TextSelectorLR<T>::TextSelectorLR() {
  font = nullptr;
  x = y = restX = activeShiftX = 0;
  aR = aG = aB = 255;
  iR = iG = iB = 127;
}

template <typename T> void menu::TextSelectorLR<T>::think() {
  SelectorLR<T>::think();
  ITextMenuElement::think();
}

template <typename T> void menu::TextSelectorLR<T>::reset() {
  SelectorLR<T>::reset();
  ITextMenuElement::reset();
}

template <typename T> void menu::TextSelectorLR<T>::draw() const {
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
    font->drawText(x, y, label + "\t" + valueStrs[this->index], r, g, b);
  }
}

template <typename T>
void menu::TextSelectorLR<T>::setValues(
    const std::vector<const T> &values,
    const std::vector<std::string> &strings) {
  this->values = values;
  this->valueStrs = strings;
}
