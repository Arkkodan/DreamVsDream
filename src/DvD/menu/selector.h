#ifndef DVD_MENU_SELECTOR_H
#define DVD_MENU_SELECTOR_H

#include "menu_interface.h"

#include "../scene/fight.h"
#include "../sound.h"

#include <string>
#include <vector>

namespace menu {
  /// @brief A selector changes a variable
  /// @details The variable is specified via template parameter.
  /// @details This implementation changes index on left or right input.
  template <typename T> class SelectorLR : virtual public IMenuElement {
  public:
    SelectorLR();

    virtual void think() override;

    virtual void doInput(uint16_t input) override final;
    virtual void doInputP(uint16_t input, uint8_t playerIndex) override final;

    virtual void reset() override;

    virtual void draw() const override;

    /// @brief Set values
    /// @details Will automatically set enabled to true if size > 0
    virtual void setValues(const std::vector<T> &values) final;

    virtual void setIndex(int index) final;
    virtual void setIndexByValue(const T &value) final;

    virtual T getValue() const final;

    virtual void setWrap(bool wrap) final;

    /// @param sndMenuLeft Sound to play when left is pressed.
    /// @param sndMenuRight Sound to play when right is pressed.
    /// If nullptr is passed, copies sndMenuLeft
    /// @param useSpeaker Whether to use a player's speaker instead of directly
    /// playing sound.
    virtual void setMenuSounds(const audio::Sound *sndMenuLeft,
                               const audio::Sound *sndMenuRight = nullptr,
                               bool useSpeaker = false) final;

  protected:
    std::vector<T> values;
    int index;
    bool wrap;

    const audio::Sound *sndMenuLeft;
    const audio::Sound *sndMenuRight;
    bool useSpeaker;
  };

  template <typename T>
  class TextSelectorLR final : virtual public SelectorLR<T>,
                               virtual public ITextMenuElement {
  public:
    TextSelectorLR();

    virtual void think() override final;

    virtual void reset() override final;

    virtual void draw() const override final;

    virtual void setLabel(const std::string &label) final;
    virtual void setValuePairs(const std::vector<T> &values,
                               const std::vector<std::string> &strings) final;

  private:
    std::string label;
    std::vector<std::string> valueStrs;
  };
} // namespace menu

#endif // DVD_MENU_SELECTOR_H

#ifndef DVD_MENU_SELECTOR_IMPLEMENTATION
#define DVD_MENU_SELECTOR_IMPLEMENTATION
#include <stdexcept>

template <typename T>
menu::SelectorLR<T>::SelectorLR()
    : values(), index(0), wrap(false), sndMenuLeft(nullptr),
      sndMenuRight(nullptr), useSpeaker(false) {
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
  if (wrap) {
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

  if (changeIndex < 0 && sndMenuLeft) {
    if (useSpeaker) {
      scene::Fight::getrPlayerAt(0).getrSpeaker().play(sndMenuLeft);
    }
    else {
      sndMenuLeft->play();
    }
  }
  else if (changeIndex > 0 && sndMenuRight) {
    if (useSpeaker) {
      scene::Fight::getrPlayerAt(0).getrSpeaker().play(sndMenuRight);
    }
    else {
      sndMenuRight->play();
    }
  }
}

template <typename T>
void menu::SelectorLR<T>::doInputP(uint16_t input, uint8_t playerIndex) {
  doInput(input);
}

template <typename T> void menu::SelectorLR<T>::reset() {}

template <typename T> void menu::SelectorLR<T>::draw() const {}

template <typename T>
void menu::SelectorLR<T>::setValues(const std::vector<T> &values) {
  if (values.empty()) {
    throw std::invalid_argument("invalid values vector: vector is empty");
  }
  this->values = values;
  this->enabled = true;
}
template <typename T> void menu::SelectorLR<T>::setIndex(int index) {
  if (index < 0 || index >= static_cast<int>(values.size())) {
    throw std::out_of_range("invalid Selector index: " + std::to_string(index));
  }
  this->index = index;
}

template <typename T>
void menu::SelectorLR<T>::setIndexByValue(const T &value) {
  auto iter = std::find(values.cbegin(), values.cend(), value);
  if (iter == values.cend()) {
    throw std::domain_error("invalid Selector value: " + std::to_string(value));
  }
  this->index = iter - values.cbegin();
}

template <typename T> T menu::SelectorLR<T>::getValue() const {
  return values[index];
}

template <typename T> void menu::SelectorLR<T>::setWrap(bool wrap) {
  this->wrap = wrap;
}

template <typename T>
void menu::SelectorLR<T>::setMenuSounds(const audio::Sound *sndMenuLeft,
                                        const audio::Sound *sndMenuRight,
                                        bool useSpeaker) {
  this->sndMenuLeft = sndMenuLeft;
  if (sndMenuRight) {
    this->sndMenuRight = sndMenuRight;
  }
  else {
    this->sndMenuRight = sndMenuLeft;
  }
  this->useSpeaker = useSpeaker;
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
void menu::TextSelectorLR<T>::setLabel(const std::string &label) {
  this->label = label;
}

template <typename T>
void menu::TextSelectorLR<T>::setValuePairs(
    const std::vector<T> &values, const std::vector<std::string> &strings) {
  if (values.empty() || strings.empty()) {
    throw std::invalid_argument(
        "invalid vectors: at least one vector is empty");
  }
  if (values.size() > strings.size()) {
    throw std::invalid_argument(
        "invalid vectors: values vector is larger than strings vector");
  }
  this->values = values;
  this->valueStrs = strings;
  this->enabled = true;
}

#endif // DVD_MENU_SELECTOR_INPLEMENTATION
