#ifndef DVD_MENU_BUTTON_H
#define DVD_MENU_BUTTON_H

#include "menu_interface.h"

#include "../sound.h"

#include <functional>
#include <string>

namespace menu {
  /// @brief A button must define an action on a press
  /// @details The action is a function that has void input and void output.
  /// @details This implementation calls the action when A is pressed.
  class ButtonA : virtual public IMenuElement {
  public:
    ButtonA();
    virtual ~ButtonA();

    virtual void think() override;

    virtual void doInput(uint16_t input) override final;
    virtual void doInputP(uint16_t input, uint8_t playerIndex) override final;

    virtual void reset() override;

    virtual void draw() const override;

    /// @brief Assign an action to perform on A
    /// @details Will automatically set enabled to true
    virtual void setAction(std::function<void(void)> action) final;

    virtual void setSelectSound(const audio::Sound *sndSelect) final;
    virtual void setInvalidSound(const audio::Sound *sndInvalid) final;

  protected:
    std::function<void(void)> action;

    const audio::Sound *sndSelect;
    const audio::Sound *sndInvalid;
  };

  /// @brief Same as ButtonA but renders text
  class TextButtonA final : virtual public ButtonA,
                            virtual public ITextMenuElement {
  public:
    TextButtonA();
    virtual ~TextButtonA();

    virtual void think() override final;

    virtual void reset() override final;

    virtual void draw() const override final;

    virtual void setText(const std::string &text) final;

  protected:
    std::string text;
  };
} // namespace menu

#endif // DVD_MENU_BUTTON_H
