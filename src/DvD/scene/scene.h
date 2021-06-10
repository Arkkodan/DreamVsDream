#ifndef DVD_SCENE_SCENE_H
#define DVD_SCENE_SCENE_H

#include "scene_base.h"

#include <memory>

#define SCENE (scene::getScene())
#define FIGHT (reinterpret_cast<scene::Fight*>(scene::scenes[scene::SCENE_FIGHT].get()))

namespace scene {
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

	extern std::array<std::unique_ptr<Scene>, SCENE_MAX> scenes;
	extern int scene;
	extern int sceneNew;

	extern Image imgLoading;

	extern float fade;
	extern bool fadeIn;
	void drawFade();

	void setScene(int _scene);

	bool input(uint16_t in);

	//Init/deinit
	void ginit();
	void deinit();

	Scene* getScene();
}

#endif // DVD_SCENE_SCENE_H
