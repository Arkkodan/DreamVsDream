#include "credits.h"

#include "scene.h"

#include "../../fileIO/text.h"
#include "../../util/fileIO.h"
#include "../player.h"
#include "../resource_manager.h"
#include "../shader_renderer/texture2D_renderer.h"
#include "../sys.h"

scene::Credits::Credits() : Scene("credits") {
  oy = 0;
  timer_start = sys::FPS * 6.35;
  timer_scroll = sys::FPS;
  secret_alpha = 0.0f;
  done = false;

  // Data
  title_r = title_g = title_b = name_r = name_g = name_b = 255;
  c_lines = 0;
  font = nullptr;
}

scene::Credits::~Credits() {}

void scene::Credits::think() {
  Scene::think();

  if (timer_start) {
    timer_start--;
  }
  else if (timer_scroll) {
    timer_scroll--;
  }
  else if (!done) {
    oy += 0.5;
  }
  else if (secret_alpha < 1.0f) {
    secret_alpha += 0.05f;
    if (secret_alpha > 1.0f) {
      secret_alpha = 1.0f;
    }
  }

  if (input(game::INPUT_A)) {
    setScene(SCENE_TITLE);
  }
}

void scene::Credits::draw() const {
  Scene::draw();

  if (!timer_start) {
    unsigned int fontH = font->getcImage()->getH();
    unsigned int logoH = logo.getH();
    if (done) {
      static std::string _sz = "Secret character unlocked!";
      font->drawText(sys::WINDOW_WIDTH - font->getTextWidth(_sz) -
                         CREDITS_OFFSET,
                     sys::FLIP(fontH * 2), _sz, 255, 255, 255, secret_alpha);
    }
    else {
      int y = (sys::WINDOW_HEIGHT - logoH) / 4 - oy;
      if (y + (int)logoH >= 0) {
        logo.draw<renderer::Texture2DRenderer>(sys::WINDOW_WIDTH - logo.getW(),
                                               y);
      }

      int oy_title = 0;

      for (int i = 0; i < c_lines; i++) {
        const char fc = lines[i].front();
        if (fc == ':') {
          oy_title++;
        }

        y = sys::WINDOW_HEIGHT + (fontH * (i + oy_title)) - oy;

        if (y + 32 < 0) {
          if (i == c_lines - 1) {
            done = true;
          }
          continue;
        }
        if (y > sys::WINDOW_HEIGHT) {
          break;
        }

        if (fc == ':') {
          std::string out = lines[i].substr(1);
          font->drawText(sys::WINDOW_WIDTH - font->getTextWidth(out) -
                             CREDITS_OFFSET,
                         y, out, title_r, title_g, title_b);
        }
        else {
          font->drawText(sys::WINDOW_WIDTH - font->getTextWidth(lines[i]) -
                             CREDITS_OFFSET,
                         y, lines[i], name_r, name_g, name_b);
        }
      }
    }
  }
}

void scene::Credits::reset() {
  Scene::reset();

  done = false;
  oy = 0;
  timer_start = sys::FPS * 6.35;
  timer_scroll = sys::FPS;
  secret_alpha = 0.0f;
}

void scene::Credits::parseLine(Parser &parser) {
  if (parser.is("LOGO", 1)) {
    logo.createFromFile(getResource(parser.getArg(1), Parser::EXT_IMAGE));
  }
  else if (parser.is("CREDITS", 2)) {
    font = getResourceT<Font>(parser.getArg(1));
    lines = fileIO::readTextAsLines(
        util::getPath(getResource(parser.getArg(2), Parser::EXT_TEXT)));
    c_lines = lines.size();
  }
  else if (parser.is("COLOR", 6)) {
    title_r = parser.getArgInt(1);
    title_g = parser.getArgInt(2);
    title_b = parser.getArgInt(3);
    name_r = parser.getArgInt(4);
    name_g = parser.getArgInt(5);
    name_b = parser.getArgInt(6);
  }
  else {
    Scene::parseLine(parser);
  }
}

void scene::Credits::parseJSON(const nlohmann::ordered_json &j_obj) {
  if (j_obj.contains("logo")) {
    logo.createFromFile(getResource(j_obj["logo"], Parser::EXT_IMAGE));
  }
  if (j_obj.contains("credits")) {
    font = getResourceT<Font>(j_obj["credits"].at("font"));
    lines = fileIO::readTextAsLines(util::getPath(
        getResource(j_obj["credits"].at("file"), Parser::EXT_TEXT)));
    c_lines = lines.size();
  }
  if (j_obj.contains("color")) {
    title_r = j_obj["color"].at("title").at("r");
    title_g = j_obj["color"].at("title").at("g");
    title_b = j_obj["color"].at("title").at("b");
    name_r = j_obj["color"].at("name").at("r");
    name_g = j_obj["color"].at("name").at("g");
    name_b = j_obj["color"].at("name").at("b");
  }
  Scene::parseJSON(j_obj);
}
