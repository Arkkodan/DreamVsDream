#ifndef DVD_MENU_BUTTON_H
#define DVD_MENU_BUTTON_H

#include "menu_interface.h"

#include "../font.h"

#include <functional>
#include <string>

namespace menu {
	/// @brief A button must define an action on a press
	/// @details The action is a function that has void input and void output.
	/// @details This implementation calls the action when A is pressed.
	class ButtonA : public IMenuElement {
	public:
		ButtonA();

		virtual void think() override;

		virtual void doInput(uint16_t input) override final;
		virtual void doInput(uint16_t input, uint8_t playerIndex) override final;

		virtual void reset() override;

		virtual void draw() const override;

		/// @brief Assign an action to perform on A
		/// @details Will automatically set enabled to true
		virtual void setAction(std::function<void(void)> action) final;

		virtual bool isActive() const final;
		virtual void setActive(bool active) final;
		virtual bool isEnabled() const final;
		virtual void setEnabled(bool enabled) final;

	protected:
		std::function<void(void)> action;

		bool active;
		bool enabled;
	};

	/// @brief Same as ButtonA but renders text
	class TextButtonA : public ButtonA {
	public:
		TextButtonA();

		virtual void think() override final;

		virtual void reset() override final;

		virtual void draw() const override final;

		virtual void setFont(Font* font) final;
		virtual void setText(const std::string& text) final;
		virtual void setPos(int restX, int y, int activeShiftX = 0) final;
		virtual void setColorActive(uint8_t aR, uint8_t aG, uint8_t aB) final;
		virtual void setColorInctive(uint8_t iR, uint8_t iG, uint8_t iB) final;

	protected:
		Font* font;
		std::string text;
		int x, y;

		int restX;
		int activeShiftX;

		uint8_t aR, aG, aB, iR, iG, iB;
	};
}

#endif // DVD_MENU_BUTTON_H
