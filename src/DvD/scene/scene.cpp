#include "scene.h"

Scene* scenes[SCENE_MAX] = { nullptr };
int scene = SCENE_INTRO;
int sceneNew = 0;

Image imgLoading;
