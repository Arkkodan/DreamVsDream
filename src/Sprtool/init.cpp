#include "../DvD/graphics.h"
#include "../DvD/error.h"
#include "../DvD/fighter.h"
#include "../DvD/sys.h"

namespace init {
	void init();
	void deinit();
}

void init::init() {
	atexit(deinit);

	sys::init();
	graphics::init(false, 0);
}

void init::deinit() {
	graphics::deinit();
	sys::deinit();
}
