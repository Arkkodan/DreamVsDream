#ifndef DVD_SCENE_SCENE_H
#define DVD_SCENE_SCENE_H

#include "scene_base.h"

#include <memory>

#define SCENE (scene::getScene())
#define FIGHT                        \
  (reinterpret_cast<scene::Fight *>( \
      scene::getSceneFromIndex(scene::SCENE_FIGHT)))

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

  void drawFade();

  void setScene(int _scene);

  bool input(uint16_t in);

  // Init/deinit
  void ginit();
  void deinit();

  Scene *getScene();
  int getSceneIndex();
  void setIMSceneIndex(int index);
  int getSceneNewIndex();
  Scene *getSceneFromIndex(int index);

  Image *getLoadingImage();

  float &getrFade();
  bool isFadeIn();
  void setFadeIn(bool fi);
} // namespace scene

#endif // DVD_SCENE_SCENE_H
