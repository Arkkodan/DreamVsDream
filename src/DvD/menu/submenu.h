#ifndef DVD_MENU_SUBMENU_H
#define DVD_MENU_SUBMENU_H

#include "menu_interface.h"

#include <functional>

namespace menu {

	/// @brief A submenu defers behavior to the most active submenu if exists
	/// @details This implementation goes up a level when B is pressed
	class Submenu : public IMenu
	{
	public:
		Submenu();

		virtual void think() override final;

		virtual void doInput(uint16_t input) override;
		virtual void doInput(uint16_t input, uint8_t playerIndex) override;

		virtual void reset() override final;

		virtual void draw() const override final;

		/// @brief Access the vector to place items.
		virtual std::vector<IMenuElement>& getElementsVector() final;

		/// @brief Set a breadcrumb submenu
		/// @param submenu A pointer to a menu.
		/// If nullptr is passed, sets this as active
		virtual void setSubmenu(IMenu* submenu) final;

		/// @brief Assign inputs that change the index
		/// @param incMask Input mask to increment the index
		/// @param decMask Input mask to decrement the index
		virtual void setInputMask(uint16_t incMask, uint16_t decMask) final;

	protected:
		std::vector<IMenuElement> elements;

		IMenu* breadcrumb;

		int elementIndex;
		uint16_t incInputMask, decInputMask;
	};

	/// @brief A main submenu performs an action on B instead
	/// @details The action is a function that has void input and void output.
	/// @details This implementation calls the action when B is pressed
	class MainSubmenuB : public Submenu {
	public:
		MainSubmenuB();

		virtual void doInput(uint16_t input) override;
		virtual void doInput(uint16_t input, uint8_t playerIndex) override;

		virtual void setAction(std::function<void(void)> action) final;

	protected:
		std::function<void(void)> action;
		bool actionSet;
	};
}

#endif // DVD_MENU_SUBMENU_H
