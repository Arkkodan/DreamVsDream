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

#define SCENE Scene::scenes[Scene::scene]
#define FIGHT (reinterpret_cast<SceneFight*>(Scene::scenes[Scene::SCENE_FIGHT].get()))

#endif // DVD_SCENE_SCENE_H
