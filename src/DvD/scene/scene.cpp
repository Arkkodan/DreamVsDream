#include "scene.h"

#include "credits.h"
#include "fight.h"
#include "intro.h"
#include "netplay.h"
#include "options.h"
#include "select.h"
#include "title.h"
#include "versus.h"

#include <glad/glad.h>

namespace scene {
  std::array<std::unique_ptr<Scene>, SCENE_MAX> scenes;
  int scene;
  int sceneNew;

  Image imgLoading;

  // Static stuff
  float fade = 1.0f;
  bool fadeIn = true;
} // namespace scene

scene::Scene *scene::getScene() { return scenes[scene].get(); }

void scene::ginit() {
  scenes[SCENE_INTRO] = std::make_unique<Intro>();
  scenes[SCENE_TITLE] = std::make_unique<Title>();
  scenes[SCENE_SELECT] = std::make_unique<Select>();
  scenes[SCENE_VERSUS] = std::make_unique<Versus>();
  scenes[SCENE_FIGHT] = std::make_unique<Fight>();
  scenes[SCENE_OPTIONS] = std::make_unique<Options>();
#ifndef NO_NETWORK
  scenes[SCENE_NETPLAY] = std::make_unique<Netplay>();
#endif
  scenes[SCENE_CREDITS] = std::make_unique<Credits>();

  scenes[SCENE_FIGHT]->init();
  scenes[SCENE_VERSUS]->init();
  scenes[SCENE_INTRO]->init();
}

void scene::deinit() {}

void scene::setScene(int _scene) {
  if (_scene == scene) {
    return;
  }
  if (_scene == SCENE_FIGHT) {
    Fight::madotsuki.reset();
    Fight::poniko.reset();
    Fight::cameraPos.x = 0;
    Fight::cameraPos.y = 0;
    Fight::idealCameraPos.x = 0;
    Fight::idealCameraPos.y = 0;
  }
  sceneNew = _scene;
  fade = 1.0f;
  fadeIn = false;
}

bool scene::input(uint16_t in) {
  return (Fight::madotsuki.frameInput & in) || (Fight::poniko.frameInput & in);
}

void scene::drawFade() {
  // Draw fade!
  glBindTexture(GL_TEXTURE_2D, 0);
  if (fadeIn) {
    glColor4f(0.0f, 0.0f, 0.0f, fade);
  }
  else {
    glColor4f(0.0f, 0.0f, 0.0f, 1.0f - fade);
  }
  glBegin(GL_QUADS);
  glVertex3f(0.0f, 0.0f, 0.0f);
  glVertex3f(0.0f, sys::WINDOW_HEIGHT, 0.0f);
  glVertex3f(sys::WINDOW_WIDTH, sys::WINDOW_HEIGHT, 0.0f);
  glVertex3f(sys::WINDOW_WIDTH, 0.0f, 0.0f);
  glEnd();
  glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
}
