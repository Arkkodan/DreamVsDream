#ifndef INPUT_H_INCLUDED
#define INPUT_H_INCLUDED

#include "globals.h"

namespace input {
	void init();

	void refresh();

	void mouseMove(int x, int y);
	void mousePress(int key, bool press);
	void keyPress(int key, bool press);
}

#endif // INPUT_H_INCLUDED
