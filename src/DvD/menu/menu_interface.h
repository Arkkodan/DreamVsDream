#ifndef DVD_MENU_MENU_INTERFACE_H
#define DVD_MENU_MENU_INTERFACE_H

#include "../player.h"

#include <vector>
#include <cstdint>

namespace menu {
	/// @brief A MenuElement object is anything that a Menu object can contain
	class IMenuElement {
	public:
		virtual void think() = 0;

		virtual void doInput(uint16_t input) = 0;
		virtual void doInput(uint16_t input, uint8_t playerIndex) = 0;

		virtual void reset() = 0;

		virtual void draw() const = 0;
	};

	/// @brief A Menu object has the same interface as a MenuElement object
	class IMenu {
	public:
		virtual void think() = 0;

		virtual void doInput(uint16_t input) = 0;
		virtual void doInput(uint16_t input, uint8_t playerIndex) = 0;

		virtual void reset() = 0;

		virtual void draw() const = 0;
	};
}

#endif // DVD_MENU_MENU_INTERFACE_H
