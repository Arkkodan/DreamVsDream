#include "submenu.h"

#include <algorithm>

menu::Submenu::Submenu()
	: elements(), breadcrumb(nullptr),
	elementIndex(0), incInputMask(0), decInputMask(0)
{}

void menu::Submenu::think() {
	if (breadcrumb) {
		// On another submenu
		breadcrumb->think();
	}
	else {
		// On this submenu
		std::for_each(elements.begin(), elements.end(),
			[](IMenuElement& e) {
				e.think();
			});
	}
}

void menu::Submenu::doInput(uint16_t input) {
	if (breadcrumb) {
		breadcrumb->doInput(input);
	}
	else {
		int changeIndex = ((input & incInputMask) ? 1 : 0) -
			((input & decInputMask) ? 1 : 0);
		if (changeIndex) {
			elementIndex += changeIndex;
			int size = elements.size();
			while (elementIndex < 0) {
				elementIndex += size;
			}
			if (elementIndex >= size) {
				elementIndex %= size;
			}
		}
		else {
			elements[elementIndex].doInput(input);
		}
	}
}

void menu::Submenu::doInput(uint16_t input, uint8_t playerIndex) {
	if (breadcrumb) {
		breadcrumb->doInput(input, playerIndex);
	}
	else {
		int changeIndex = ((input & incInputMask) ? 1 : 0) -
			((input & decInputMask) ? 1 : 0);
		if (changeIndex) {
			elementIndex += changeIndex;
			int size = elements.size();
			while (elementIndex < 0) {
				elementIndex += size;
			}
			if (elementIndex >= size) {
				elementIndex %= size;
			}
		}
		else {
			elements[elementIndex].doInput(input, playerIndex);
		}
	}
}

void menu::Submenu::reset() {
	if (breadcrumb) {
		breadcrumb->reset();
	}

	setSubmenu(nullptr);
	elementIndex = 0;
}

void menu::Submenu::draw() const {
	if (breadcrumb) {
		breadcrumb->draw();
	}
	else {
		std::for_each(elements.cbegin(), elements.cend(),
			[](const IMenuElement& e) {
				e.draw();
			});
	}
}

std::vector<menu::IMenuElement>& menu::Submenu::getElementsVector() {
	return elements;
}

void menu::Submenu::setSubmenu(IMenu* submenu) {
	breadcrumb = submenu;
}

void menu::Submenu::setInputMask(uint16_t incMask, uint16_t decMask) {
	incInputMask = incMask;
	decInputMask = decMask;
}

menu::MainSubmenuB::MainSubmenuB()
	: action(), actionSet(false)
{}

void menu::MainSubmenuB::doInput(uint16_t input) {
	if (breadcrumb) {
		breadcrumb->doInput(input);
	}
	else {
		if (actionSet && (input & game::INPUT_B)) {
			action();
		}
		else {
			Submenu::doInput(input);
		}
	}
}

void menu::MainSubmenuB::doInput(uint16_t input, uint8_t playerIndex) {
	if (breadcrumb) {
		breadcrumb->doInput(input, playerIndex);
	}
	else {
		if (actionSet && (input & game::INPUT_B)) {
			action();
		}
		else {
			Submenu::doInput(input, playerIndex);
		}
	}
}

void menu::MainSubmenuB::setAction(std::function<void(void)> action) {
	this->action = action;
	actionSet = true;
}
