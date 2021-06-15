#include "stage.h"

#include "../fileIO/json.h"
#include "../util/fileIO.h"
#include "../util/rng.h"
#include "error.h"
#include "graphics.h"
#include "image.h"
#include "parser.h"
#include "resource_manager.h"
#include "shader_renderer/texture2D_renderer.h"

#include <algorithm>

int Stage::stage = -1;
std::vector<Stage *> Stage::stages;

void Stage::ginit() {
  resource_manager::loadFromManifest<Stage>();
  stages = resource_manager::getFromManifest<Stage>();
}

void Stage::deinit() {}

Stage::Stage() {
  exists = initialized = false;

  width = height = widthAbs = heightAbs = 0;
}

/*Stage::Stage(const char* _name)
{

}*/

Stage::~Stage() {}

void Stage::create(std::string _name) {
  this->name = std::move(_name);

  thumbnail = Animation("stages/" + name + "/thumbnail.gif");

  exists = true;
}

void Stage::init() {
  initialized = true;
  // Load the stage data from the file
  bool jsonSuccess = false;
  try {
    jsonSuccess = parseJSON(name);
  }
  catch (const nlohmann::detail::out_of_range &e) {
    error::error(name + "/stage.json error: " + e.what());
    imagesAbove.clear();
    imagesBelow.clear();
  }
  if (!jsonSuccess) {
    error::error("Cannot read " + name + "/stage.json. Falling back to " +
                 name + "/stage.ubu");
    parseFile("stages/" + name + "/stage.ubu");
  }
}

void Stage::think() {
  if (!initialized) {
    init();
  }
  std::for_each(imagesAbove.begin(), imagesAbove.end(),
                [](scene::SceneImage &si) { si.think(); });
  std::for_each(imagesBelow.begin(), imagesBelow.end(),
                [](scene::SceneImage &si) { si.think(); });
}

void Stage::draw(bool _above) const {
  if (_above) {
    renderer::Texture2DRenderer::setColor(1.0f, 1.0f, 1.0f, 0.5f);
    std::for_each(imagesAbove.cbegin(), imagesAbove.cend(),
                  [](const scene::SceneImage &si) { si.draw(true); });
  }
  else {
    std::for_each(imagesBelow.cbegin(), imagesBelow.cend(),
                  [](const scene::SceneImage &si) { si.draw(true); });
  }
}

void Stage::reset() {
  std::for_each(imagesAbove.begin(), imagesAbove.end(),
                [](scene::SceneImage &si) { si.reset(); });
  std::for_each(imagesBelow.begin(), imagesBelow.end(),
                [](scene::SceneImage &si) { si.reset(); });
}

void Stage::parseFile(const std::string &szFileName) {
  Parser parser;
  std::string path = util::getPath(szFileName);
  if (!parser.open(path)) {
    error::die("Cannot parse file \"" + path + "\"");
  }

  // Get all the data
  while (parser.parseLine()) {
    int argc = parser.getArgC();

    // Parse it
    bool above = parser.is("IMAGE_A", 2);
    if (above || parser.is("IMAGE_B", 2)) {
      float parallax = parser.getArgFloat(2);
      int round = 0;
      float xvel = 0.0f;
      float yvel = 0.0f;
      bool wrap = false;
      if (argc > 3) {
        round = parser.getArgInt(3);
        if (argc > 4) {
          xvel = parser.getArgFloat(4);
          if (argc > 5) {
            yvel = parser.getArgFloat(5);
            if (argc > 6) {
              wrap = parser.getArgBool(6, false);
            }
          }
        }
      }

      // Add a new image
      Image imgData;
      imgData.createFromFile(getResource(parser.getArg(1), "png"));
      if (!imgData.exists()) {
        continue;
      }
      if (above) {
        imagesAbove.emplace_back(imgData, 0.0f, 0.0f, parallax,
                                 Image::Render::NORMAL, xvel, yvel, wrap,
                                 round);
      }
      else {
        imagesBelow.emplace_back(imgData, 0.0f, 0.0f, parallax,
                                 Image::Render::NORMAL, xvel, yvel, wrap,
                                 round);
      }
    }
    else if (parser.is("WIDTH", 1)) {
      width = parser.getArgInt(1);
    }
    else if (parser.is("HEIGHT", 1)) {
      height = parser.getArgInt(1);
    }
    else if (parser.is("WIDTH_ABS", 1)) {
      widthAbs = parser.getArgInt(1);
    }
    else if (parser.is("HEIGHT_ABS", 1)) {
      heightAbs = parser.getArgInt(1);
    }
    else if (parser.is("BGM", 1)) {
      if (argc > 2) {
        bgm.createFromFile(getResource(parser.getArg(1), Parser::EXT_MUSIC),
                           getResource(parser.getArg(2), Parser::EXT_MUSIC));
      }
      else {
        bgm.createFromFile("",
                           getResource(parser.getArg(1), Parser::EXT_MUSIC));
      }
    }
    else if (parser.is("BGM2", 1)) {
      if (argc > 2) {
        bgm2.createFromFile(getResource(parser.getArg(1), Parser::EXT_MUSIC),
                            getResource(parser.getArg(2), Parser::EXT_MUSIC));
      }
      else {
        bgm2.createFromFile("",
                            getResource(parser.getArg(1), Parser::EXT_MUSIC));
      }
    }
  }
}

bool Stage::parseJSON(const std::string &name) {
  const auto &j_obj =
      fileIO::readJSON(util::getPath("stages/" + name + "/stage.json"));
  if (!j_obj.is_object()) {
    return false;
  }

  if (j_obj.contains("imagesAhead")) {
    for (const auto &image : j_obj["imagesAhead"]) {
      Image imgData;
      imgData.createFromFile(getResource(image["image"], Parser::EXT_IMAGE));
      if (imgData.exists()) {
        float parallax = image.at("parallax");
        int round = image.value("round", 0);
        float xvel = 0.0f;
        float yvel = 0.0f;
        if (image.contains("vel")) {
          xvel = image["vel"].value("x", 0.0f);
          yvel = image["vel"].value("y", 0.0f);
        }
        bool wrap = image.value("wrap", false);
        imagesAbove.emplace_back(imgData, 0.0f, 0.0f, parallax,
                                 Image::Render::NORMAL, xvel, yvel, wrap,
                                 round);
      }
    }
  }
  if (j_obj.contains("imagesBehind")) {
    for (const auto &image : j_obj["imagesBehind"]) {
      Image imgData;
      imgData.createFromFile(getResource(image["image"], Parser::EXT_IMAGE));
      if (imgData.exists()) {
        float parallax = image.at("parallax");
        int round = image.value("round", 0);
        float xvel = 0.0f;
        float yvel = 0.0f;
        if (image.contains("vel")) {
          xvel = image["vel"].value("x", 0.0f);
          yvel = image["vel"].value("y", 0.0f);
        }
        bool wrap = image.value("wrap", false);
        imagesBelow.emplace_back(imgData, 0.0f, 0.0f, parallax,
                                 Image::Render::NORMAL, xvel, yvel, wrap,
                                 round);
      }
    }
  }
  if (j_obj.contains("entities")) {
    width = j_obj["entities"].at("w_bounds");
    height = j_obj["entities"].at("h_ground");
  }
  if (j_obj.contains("camera")) {
    widthAbs = j_obj["camera"].at("w");
    heightAbs = j_obj["camera"].at("h");
  }
  if (j_obj.contains("bgm")) {
    for (int i = 0, size = j_obj["bgm"].size(); i < size; i++) {
      const auto &j_bgm = j_obj["bgm"][i];
      std::string loop = getResource(j_bgm.at("loop"), Parser::EXT_MUSIC);
      std::string intro = j_bgm.value("intro", "");
      switch (i) {
      case 0:
        bgm.createFromFile(
            (intro.empty() ? "" : getResource(intro, Parser::EXT_MUSIC)), loop);
        break;
      case 1:
        bgm2.createFromFile(
            (intro.empty() ? "" : getResource(intro, Parser::EXT_MUSIC)), loop);
        break;
      }
    }
  }

  return true;
}

std::string Stage::getResource(const std::string &resource,
                               const std::string &extension) const {
  if (*resource.c_str() == '*') {
    return "stages/common/" + resource.substr(1, std::string::npos) + "." +
           extension;
  }
  else {
    return "stages/" + name + "/" + resource + "." + extension;
  }
}

void Stage::bgmPlay() {
  if (bgm.exists()) {
    if (bgm2.exists()) {
      if (util::roll(2)) {
        bgm.play();
      }
      else {
        bgm2.play();
      }
    }
    else {
      bgm.play();
    }
  }
}

bool Stage::isExists() const { return exists; }
