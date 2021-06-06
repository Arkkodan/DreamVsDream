#include "button.h"

menu::ButtonA::ButtonA()
	: action(), active(false), enabled(false)
{}

void menu::ButtonA::think() {}

void menu::ButtonA::doInput(uint16_t input) {
	if (enabled && (input & game::INPUT_A)) {
		action();
	}
}

void menu::ButtonA::doInput(uint16_t input, uint8_t playerIndex) {
	doInput(input);
}

void menu::ButtonA::reset() {}

void menu::ButtonA::draw() const {}

void menu::ButtonA::setAction(std::function<void(void)> action) {
	this->action = action;
	enabled = true;
}

bool menu::ButtonA::isActive() const {
	return active;
}
void menu::ButtonA::setActive(bool active) {
	this->active = active;
}

bool menu::ButtonA::isEnabled() const {
	return enabled;
}
void menu::ButtonA::setEnabled(bool enabled) {
	this->enabled = enabled;
}

menu::TextButtonA::TextButtonA()
	: font(nullptr), text("")
{
	x = y = restX = activeShiftX = 0;
	aR = aG = aB = 255;
	iR = iG = iB = 127;
}

void menu::TextButtonA::think() {
	if (active) {
		if (x != restX + activeShiftX) {
			x += (restX + activeShiftX - x) / 2;
			if (abs(restX + activeShiftX - x) <= 1) {
				x = restX + activeShiftX;
			}
		}
	}
	else {
		if (x != restX) {
			x += (restX - x) / 2;
			if (abs(restX - x) <= 1) {
				x = restX;
			}
		}
	}
}

void menu::TextButtonA::reset() {
	x = restX;
}

void menu::TextButtonA::draw() const {
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
		font->drawText(x, y, text, r, g, b);
	}
}

void menu::TextButtonA::setFont(Font* font) {
	if (font && font->exists()) {
		this->font = font;
	}
}

void menu::TextButtonA::setText(const std::string& text) {
	this->text = text;
}
void menu::TextButtonA::setPos(int restX, int y, int activeShiftX) {
	this->restX = restX;
	this->y = y;
	this->activeShiftX = activeShiftX;
}
void menu::TextButtonA::setColorActive(uint8_t aR, uint8_t aG, uint8_t aB) {
	this->aR = aR;
	this->aG = aG;
	this->aB = aB;
}
void menu::TextButtonA::setColorInctive(uint8_t iR, uint8_t iG, uint8_t iB) {
	this->iR = iR;
	this->iG = iG;
	this->iB = iB;
}
