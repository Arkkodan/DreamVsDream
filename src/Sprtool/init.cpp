#include "../DvD/globals.h"
#include "../DvD/graphics.h"
#include "../DvD/error.h"
#include "../DvD/fighter.h"

namespace init {
	void init();
	void deinit();
}

void init::init() {
	atexit(deinit);

	os::init();
	graphics::init(false, 0);
}

void init::deinit() {
	graphics::deinit();
	os::deinit();
}
