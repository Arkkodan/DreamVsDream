#ifndef DVD_MENU_SUBMENU_H
#define DVD_MENU_SUBMENU_H

#include "menu_interface.h"

#include "../sound.h"

#include <functional>
#include <list>
#include <memory>

namespace menu {

  /// @brief A submenu contains menu elements
  class Submenu : public IMenu {
  public:
    Submenu();

    virtual void think() override;

    virtual void doInput(uint16_t input) override;
    virtual void doInput(uint16_t input, uint8_t playerIndex) override;

    virtual void reset() override final;

    virtual void draw() const override;

    virtual void
    setElements(std::vector<std::unique_ptr<IMenuElement>> &&elements);

    /// @brief Assign inputs that change the index
    /// @param incMask Input mask to increment the index
    /// @param decMask Input mask to decrement the index
    virtual void setInputMask(uint16_t incMask, uint16_t decMask) final;

    virtual void setMenuSound(const audio::Sound *sndMenu) final;

  protected:
    std::vector<std::unique_ptr<IMenuElement>> elements;

    int elementIndex;
    uint16_t incInputMask, decInputMask;

    const audio::Sound *sndMenu;
  };

  /// @brief A main submenu performs an action on B instead
  /// @details If the active submenu is another submenu, go up a level.
  /// @details If the active submenu is this, call the action.
  /// @details The action is a function that has void input and void output.
  class MainSubmenuB : public Submenu {
  public:
    MainSubmenuB();

    virtual void think() override final;

    /// @brief Push a breadcrumb submenu
    /// @param submenu A pointer to a menu.
    /// If nullptr is passed, does nothing
    virtual void pushSubmenu(Submenu *submenu) final;
    /// @brief Pop a breadcrumb submenu
    virtual void popSubmenu() final;

    virtual void doInput(uint16_t input) override final;
    virtual void doInput(uint16_t input, uint8_t playerIndex) override final;

    virtual void draw() const override final;

    virtual void setAction(std::function<void(void)> action) final;

    virtual void setBackSound(const audio::Sound *sndMenu) final;

  protected:
    std::list<Submenu *> breadcrumbs;

    std::function<void(void)> action;
    bool actionSet;

    const audio::Sound *sndBack;
  };
} // namespace menu

#endif // DVD_MENU_SUBMENU_H
