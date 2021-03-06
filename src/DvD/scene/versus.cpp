#include "versus.h"

#include "scene.h"

#include "../graphics.h"
#include "../player.h"
#include "../shader_renderer/texture2D_renderer.h"
#include "../sys.h"

// Scene Versus
scene::Versus::Versus() : Scene("versus") {
  // video = nullptr;

  timer1 = timer2 = timer3 = timer4 = timer5 = timer6 = 0;
  timerF = 0.0f;

  portraits[0] = portraits[1] = nullptr;
}

scene::Versus::~Versus() {}

void scene::Versus::init() { Scene::init(); }

void scene::Versus::think() {
  Scene::think();

  if (input(game::INPUT_A))
    setScene(SCENE_FIGHT);

  if (timer1 > 0) {
    timer1--;
  }
  else if (timer2 > 0) {
    timer2 = static_cast<int>(timer2 * 0.90);
  }
  else if (timer3 > 0) {
    timer3--;
  }
  else if (timer4 > 0) {
    timer4 = static_cast<int>(timer4 * 0.90);
  }
  else if (timer5 > 0) {
    timer5--;
  }
  else if (timer6 > 0) {
    timer6--;
    if (!timer6) {
      setScene(SCENE_FIGHT);
    }
  }

  if (!timer5) {
    timer2 -= 100;
  }

  if (!timer1) {
    timerF += 0.03f;
  }
}

void scene::Versus::reset() {
  Scene::reset();

  timer1 = sys::FPS * 2;
  timer2 = 1000;
  timer3 = sys::FPS * 1;
  timer4 = 1000;
  timer5 = sys::FPS * 2;
  timer6 = static_cast<int>(sys::FPS * 1.5);

  timerF = 0.0f;
}

void scene::Versus::draw() const {
  Scene::draw();

  if (portraits[1]) {
    renderer::Texture2DRenderer::setColor(
        {1.0f, 1.0f, 1.0f,
         0.5f}); // MIN(1.0f, (FPS * 2 + FPS / 2 - timer6) / 60.0f))
    graphics::setScale(2.0f);
    unsigned int p1W = portraits[1]->getW();
    portraits[1]->draw<renderer::Texture2DRenderer>(
        static_cast<int>(sys::WINDOW_WIDTH - p1W * 1.5 - timer2 + timerF), -120,
        true);
    portraits[1]->draw<renderer::Texture2DRenderer>(
        static_cast<int>(sys::WINDOW_WIDTH - p1W + timer4 - timerF), 0, true);
  }

  if (portraits[0]) {
    renderer::Texture2DRenderer::setColor(
        {1.0f, 1.0f, 1.0f,
         0.5f}); // MIN(1.0f, (FPS * 2 + FPS / 2 - timer6) / 60.0f));
    graphics::setScale(2.0f);
    unsigned int p0W = portraits[0]->getW();
    portraits[0]->draw<renderer::Texture2DRenderer>(
        static_cast<int>(p0W * -0.5 + timer2 - timerF), -120);
    portraits[0]->draw<renderer::Texture2DRenderer>(
        static_cast<int>(0 - timer4 + timerF), 0);
  }
}

void scene::Versus::parseLine(Parser &parser) { Scene::parseLine(parser); }

void scene::Versus::setPortraitAt(int index, const Image *portrait) {
  portraits[index] = portrait;
}
