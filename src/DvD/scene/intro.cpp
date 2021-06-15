#include "intro.h"

#include "scene.h"

#include "../error.h"
#include "../graphics.h"
#include "../player.h"
#include "../resource_manager.h"
#include "../shader_renderer/texture2D_renderer.h"
#include "../sys.h"

scene::Intro::Intro() : Scene("intro") {
  timer = sys::FPS / 2;
  state = 0;
  sfx = nullptr;
}

scene::Intro::~Intro() {}

void scene::Intro::think() {
  Scene::think();

  if (timer == sys::FPS / 2) {
    sfx->play();
  }

  if (input(game::INPUT_A) && !timer) {
    timer = sys::FPS / 2;
    state++;
  }

  if (timer) {
    timer--;
    if (!timer && state % 2 == 1) {
      timer = sys::FPS / 2;
      state++;
      if (state == 6 && graphics::shader_support) {
        timer = 0;
        setScene(SCENE_TITLE);
      }
      if (state == 8) {
        timer = 0;
        setScene(SCENE_TITLE);
      }
    }
  }
}

void scene::Intro::draw() const {
  // Draw our own fade
  float _alpha = timer / (float)(sys::FPS / 2);
  if (state % 2 == 0) {
    _alpha = 1.0 - _alpha;
  }
  renderer::Texture2DRenderer::setColor(1.0f, 1.0f, 1.0f, _alpha);
  if (state < 2) {
    renderer::Texture2DRenderer::setColor(1.0f, 1.0f, 1.0f, _alpha);
    instructions.draw<renderer::Texture2DRenderer>(0, 0);
  }
  else if (state < 4) {
    renderer::Texture2DRenderer::setColor(1.0f, 1.0f, 1.0f, _alpha);
    disclaimer_en.draw<renderer::Texture2DRenderer>(0, 0);
  }
  else if (state < 6) {
    renderer::Texture2DRenderer::setColor(1.0f, 1.0f, 1.0f, _alpha);
    disclaimer_ja.draw<renderer::Texture2DRenderer>(0, 0);
  }
  else if (state < 8 && !graphics::shader_support) {
    renderer::Texture2DRenderer::setColor(1.0f, 1.0f, 1.0f, _alpha);
    shader_error.draw<renderer::Texture2DRenderer>(0, 0);
  }
}

void scene::Intro::parseLine(Parser &parser) {
  if (parser.is("SFX", 1)) {
    sfx = getResourceT<audio::Sound>(parser.getArg(1));
  }
  else if (parser.is("INSTRUCTIONS", 1)) {
    instructions.createFromFile(
        getResource(parser.getArg(1), Parser::EXT_IMAGE));
  }
  else if (parser.is("DISCLAIMER", 2)) {
    disclaimer_en.createFromFile(
        getResource(parser.getArg(1), Parser::EXT_IMAGE));
    disclaimer_ja.createFromFile(
        getResource(parser.getArg(2), Parser::EXT_IMAGE));
  }
  else if (parser.is("SHADER_ERROR", 1)) {
    shader_error.createFromFile(
        getResource(parser.getArg(1), Parser::EXT_IMAGE));
  }
  else {
    Scene::parseLine(parser);
  }
}

void scene::Intro::parseJSON(const nlohmann::ordered_json &j_obj) {
  if (j_obj.contains("sfx")) {
    sfx = getResourceT<audio::Sound>(j_obj["sfx"]);
  }
  if (j_obj.contains("instructions")) {
    instructions.createFromFile(
        getResource(j_obj["instructions"], Parser::EXT_IMAGE));
  }
  if (j_obj.contains("disclaimer")) {
    disclaimer_en.createFromFile(
        getResource(j_obj["disclaimer"].at("en"), Parser::EXT_IMAGE));
    disclaimer_ja.createFromFile(
        getResource(j_obj["disclaimer"].at("ja"), Parser::EXT_IMAGE));
  }
  if (j_obj.contains("shader_error")) {
    shader_error.createFromFile(
        getResource(j_obj["shader_error"], Parser::EXT_IMAGE));
  }
  Scene::parseJSON(j_obj);
}
