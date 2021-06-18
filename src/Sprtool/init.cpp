#include "../DvD/error.h"
#include "../DvD/fighter.h"
#include "../DvD/graphics.h"
#include "../DvD/sys.h"

namespace init {
  void init();
  void deinit();
} // namespace init

void init::init() {
  atexit(deinit);

  sys::init();
  graphics::init(false, 0);
}

void init::deinit() {
  graphics::deinit();
  sys::deinit();
}
