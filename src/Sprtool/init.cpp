#include "../DvD/globals.h"
#include "../DvD/graphics.h"
#include "../DvD/error.h"
#include "../DvD/fighter.h"

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
