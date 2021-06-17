#include "scene.h"

#include "credits.h"
#include "fight.h"
#include "intro.h"
#include "netplay.h"
#include "options.h"
#include "select.h"
#include "title.h"
#include "versus.h"

#include "../shader_renderer/primitive_renderer.h"

namespace scene {
  static std::array<std::unique_ptr<Scene>, SCENE_MAX> scenes;
  static int scene;
  static int sceneNew;

  static Image imgLoading;

  // Static stuff
  static float fade = 1.0f;
  static bool fadeIn = true;
} // namespace scene

scene::Scene *scene::getScene() { return scenes[scene].get(); }
int scene::getSceneIndex() { return scene; }
void scene::setIMSceneIndex(int index) { scene = index; }
int scene::getSceneNewIndex() { return sceneNew; }
scene::Scene *scene::getSceneFromIndex(int index) {
  return scenes[index].get();
}

Image *scene::getLoadingImage() { return &imgLoading; }

float &scene::getrFade() { return fade; }
bool scene::isFadeIn() { return fadeIn; }
void scene::setFadeIn(bool fi) { fadeIn = fi; }

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
  return (Fight::madotsuki.getFrameInput() & in) ||
         (Fight::poniko.getFrameInput() & in);
}

void scene::drawFade() {
  // Draw fade!
  if (fadeIn) {
    renderer::PrimitiveRenderer::setColor(0.0f, 0.0f, 0.0f, fade);
  }
  else {
    renderer::PrimitiveRenderer::setColor(0.0f, 0.0f, 0.0f, 1.0f - fade);
  }
  renderer::PrimitiveRenderer::setPosRect(0.0f, sys::WINDOW_WIDTH,
                                          sys::WINDOW_HEIGHT, 0.0f);
  renderer::PrimitiveRenderer::draw();
  renderer::ShaderProgram::unuse();
}
