#include "../../DvD/src/globals.h"
#include "../../DvD/src/graphics.h"
#include "../../DvD/src/error.h"
#include "../../DvD/src/fighter.h"

void init() {
	void deinit();
	atexit(deinit);

	os::init();
	graphics::init(false, 0);
}

void deinit() {
	graphics::deinit();
	os::deinit();
}
