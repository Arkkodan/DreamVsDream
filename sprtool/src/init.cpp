#include "../../DvD/src/globals.h"
#include "../../DvD/src/graphics.h"
#include "../../DvD/src/error.h"
#include "../../DvD/src/fighter.h"

void init()
{
    void deinit();
    atexit(deinit);

    OS::init();
    Graphics::init(false, 0);
}

void deinit()
{
    Graphics::deinit();
    OS::deinit();
}
