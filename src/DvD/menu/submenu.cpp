#include "submenu.h"

#include "button.h"
#include "selector.h"

#include <algorithm>

menu::Submenu::Submenu()
    : elements(), elementIndex(0), incInputMask(0), decInputMask(0),
      sndMenu(nullptr) {}

void menu::Submenu::think() {
  for (auto &e : elements) {
    e->think();
  }
}

void menu::Submenu::doInput(uint16_t input) {
  if (elements.empty()) {
    return;
  }
  int changeIndex =
      ((input & incInputMask) ? 1 : 0) - ((input & decInputMask) ? 1 : 0);
  if (changeIndex) {
    elements[elementIndex]->setActive(false);
    elementIndex += changeIndex;
    int size = elements.size();
    while (elementIndex < 0) {
      elementIndex += size;
    }
    if (elementIndex >= size) {
      elementIndex %= size;
    }
    elements[elementIndex]->setActive(true);
    if (sndMenu) {
      sndMenu->play();
    }
  }
  else {
    elements[elementIndex]->doInput(input);
  }
}

void menu::Submenu::doInputP(uint16_t input, uint8_t playerIndex) {
  if (elements.empty()) {
    return;
  }
  int changeIndex =
      ((input & incInputMask) ? 1 : 0) - ((input & decInputMask) ? 1 : 0);
  if (changeIndex) {
    elements[elementIndex]->setActive(false);
    elementIndex += changeIndex;
    int size = elements.size();
    while (elementIndex < 0) {
      elementIndex += size;
    }
    if (elementIndex >= size) {
      elementIndex %= size;
    }
    elements[elementIndex]->setActive(true);
    if (sndMenu) {
      sndMenu->play();
    }
  }
  else {
    elements[elementIndex]->doInputP(input, playerIndex);
  }
}

void menu::Submenu::reset() {
  for (auto &e : elements) {
    e->setActive(false);
    if (e == elements[elementIndex]) {
      e->setActive(true);
    }
    e->reset();
  }
}

void menu::Submenu::draw() const {
  for (const auto &e : elements) {
    e->draw();
  }
}

void menu::Submenu::setElements(
    std::vector<std::unique_ptr<IMenuElement>> &&elements) {
  this->elements = std::move(elements);
}

void menu::Submenu::setInputMask(uint16_t incMask, uint16_t decMask) {
  incInputMask = incMask;
  decInputMask = decMask;
}

void menu::Submenu::setMenuSound(const audio::Sound *sndMenu) {
  this->sndMenu = sndMenu;
}

int menu::Submenu::getIndex() const { return elementIndex; }
void menu::Submenu::setIndex(int index) {
  elements[this->elementIndex]->setActive(false);
  this->elementIndex = index;
  elements[this->elementIndex]->setActive(true);
}

const menu::IMenuElement *menu::Submenu::getcElementAt(int index) const {
  return elements[index].get();
}
menu::IMenuElement *menu::Submenu::getElementAt(int index) {
  return elements[index].get();
}

menu::MainSubmenuB::MainSubmenuB()
    : breadcrumbs(), action(), actionSet(false), sndBack(nullptr) {}

void menu::MainSubmenuB::think() {
  if (breadcrumbs.empty()) {
    Submenu::think();
  }
  else {
    breadcrumbs.back()->think();
  }
}

void menu::MainSubmenuB::pushSubmenu(Submenu *submenu) {
  breadcrumbs.push_back(submenu);
}

void menu::MainSubmenuB::popSubmenu() {
  breadcrumbs.pop_back();
  if (sndBack) {
    sndBack->play();
  }
}

void menu::MainSubmenuB::doInput(uint16_t input) {
  if (input & game::INPUT_B) {
    // On B input, perform the action or pop breadcrumbs
    if (breadcrumbs.empty()) {
      if (actionSet) {
        action();
      }
    }
    else {
      breadcrumbs.pop_back();
    }
  }
  else {
    // Defer input to the active submenu
    if (breadcrumbs.empty()) {
      Submenu::doInput(input);
    }
    else {
      breadcrumbs.back()->doInput(input);
    }
  }
}

void menu::MainSubmenuB::doInputP(uint16_t input, uint8_t playerIndex) {
  if (input & game::INPUT_B) {
    if (breadcrumbs.empty()) {
      if (actionSet) {
        action();
      }
    }
    else {
      breadcrumbs.pop_back();
      if (sndBack) {
        sndBack->play();
      }
    }
  }
  else {
    if (breadcrumbs.empty()) {
      Submenu::doInputP(input, playerIndex);
    }
    else {
      breadcrumbs.back()->doInputP(input, playerIndex);
    }
  }
}

void menu::MainSubmenuB::draw() const {
  if (breadcrumbs.empty()) {
    Submenu::draw();
  }
  else {
    breadcrumbs.back()->draw();
  }
}

void menu::MainSubmenuB::setAction(std::function<void(void)> action) {
  this->action = action;
  actionSet = true;
}

void menu::MainSubmenuB::setBackSound(const audio::Sound *sndBack) {
  this->sndBack = sndBack;
}

int menu::MainSubmenuB::getActiveSubmenuIndex() const {
  if (breadcrumbs.empty()) {
    return elementIndex;
  }
  else {
    return breadcrumbs.back()->getIndex();
  }
}
