#ifndef DVD_SCENE_SCENE_H
#define DVD_SCENE_SCENE_H

#include "scene_base.h"
#include "intro.h"
#include "title.h"
#include "select.h"
#include "versus.h"
#include "options.h"
#include "fight.h"
#include "netplay.h"
#include "credits.h"

#include "../player.h"
#include "../image.h"

namespace g_main {
	extern game::Player madotsuki;
	extern game::Player poniko;
}

#define SCENE Scene::scenes[Scene::scene]
#define FIGHT ((SceneFight*)Scene::scenes[Scene::SCENE_FIGHT])

#endif // DVD_SCENE_SCENE_H
