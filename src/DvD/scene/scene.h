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

enum {
	SCENE_FIGHT,

	SCENE_INTRO,

	SCENE_TITLE,
	SCENE_SELECT,
	SCENE_VERSUS,

	SCENE_OPTIONS,

#ifndef NO_NETWORK
	SCENE_NETPLAY,
#endif

	SCENE_CREDITS,

	SCENE_MAX,

	SCENE_QUIT, // Dummy scene for quitting
};

extern Scene* scenes[SCENE_MAX];
extern int scene;
extern int sceneNew;

extern game::Player madotsuki;
extern game::Player poniko;

#define SCENE scenes[scene]
#define FIGHT ((SceneFight*)scenes[SCENE_FIGHT])

extern Image imgLoading;

#endif // DVD_SCENE_SCENE_H
