#ifndef DVD_MENU_MENU_INTERFACE_H
#define DVD_MENU_MENU_INTERFACE_H

#include "../font.h"
#include "../player.h"

#include <cstdint>
#include <vector>

namespace menu {
  /// @brief A MenuElement object is anything that a Menu object can contain
  class IMenuElement {
  protected:
    IMenuElement();

  public:
    virtual ~IMenuElement();

    virtual void think(); // override

    virtual void doInput(uint16_t input);                       // override
    virtual void doInputP(uint16_t input, uint8_t playerIndex); // override

    virtual void reset(); // override

    virtual void draw() const; // override

    virtual bool isActive() const final;
    virtual void setActive(bool active) final;
    virtual bool isEnabled() const final;
    virtual void setEnabled(bool enabled) final;

  protected:
    bool active = false;
    bool enabled = false;
  };

  class ITextMenuElement : virtual public IMenuElement {
  protected:
    ITextMenuElement();

  public:
    virtual ~ITextMenuElement();

    virtual void think() override;

    virtual void reset() override;

    virtual void setFont(Font *font) final;
    virtual void setPos(int restX, int y, int activeShiftX = 0) final;
    virtual void setColorActive(uint8_t aR, uint8_t aG, uint8_t aB) final;
    virtual void setColorInctive(uint8_t iR, uint8_t iG, uint8_t iB) final;

  protected:
    Font *font = nullptr;

    int x = 0, y = 0;

    int restX = 0;
    int activeShiftX = 0;

    uint8_t aR = 0, aG = 0, aB = 0, iR = 0, iG = 0, iB = 0;
  };

  /// @brief A Menu object has the same interface as a MenuElement object
  class IMenu {
  public:
    virtual ~IMenu();

    virtual void think() = 0;

    virtual void doInput(uint16_t input) = 0;
    virtual void doInputP(uint16_t input, uint8_t playerIndex) = 0;

    virtual void reset() = 0;

    virtual void draw() const = 0;
  };
} // namespace menu

#endif // DVD_MENU_MENU_INTERFACE_H
