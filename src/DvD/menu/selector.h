#ifndef DVD_MENU_SELECTOR_H
#define DVD_MENU_SELECTOR_H

#include "menu_interface.h"

#include <string>
#include <vector>

namespace menu {
  /// @brief A selector changes a variable
  /// @details The variable is specified via template parameter.
  /// @details This implementation changes index on left or right input.
  template <typename T> class SelectorLR : public IMenuElement {
  public:
    SelectorLR();

    virtual void think() override;

    virtual void doInput(uint16_t input) override final;
    virtual void doInput(uint16_t input, uint8_t playerIndex) override final;

    virtual void reset() override;

    virtual void draw() const override;

    /// @brief Set values
    /// @details Will automatically set enabled to true if size > 0
    virtual void setValues(const std::vector<const T> &values) final;

    virtual void setIndex(int index) final;

    virtual T getValue() const final;

    virtual void setWrapIndex(bool wrapIndex) final;

    virtual bool isActive() const final;
    virtual void setActive(bool active) final;
    virtual bool isEnabled() const final;
    virtual void setEnabled(bool enabled) final;

  protected:
    std::vector<const T> values;
    int index;
    bool wrapIndex;
  };

  template <typename T>
  class TextSelectorLR final : public SelectorLR<T>, public ITextMenuElement {
    TextSelectorLR();

    virtual void think() override final;

    virtual void reset() override final;

    virtual void draw() const override final;

    virtual void setValues(const std::vector<const T> &values) = delete;
    virtual void setValues(const std::vector<const T> &values,
                           const std::vector<std::string> &strings) final;

  private:
    std::string label;
    std::vector<std::string> valueStrs;
  };
} // namespace menu

#endif // DVD_MENU_SELECTOR_H
