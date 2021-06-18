#include "scene_base.h"

#include "fight.h"
#include "scene.h"

#include "../../fileIO/json.h"
#include "../../util/fileIO.h"
#include "../error.h"
#include "../graphics.h"
#include "../resource_manager.h"
#include "../shader_renderer/texture2D_renderer.h"
#include "../stage.h"
#include "../sys.h"

#include <algorithm>

// SCENES
scene::Scene::Scene(std::string name_) {
  // Copy the name.
  name = name_;

  initialized = false;
  bgmPlaying = false;
  // video = nullptr;

  sndMenu = sndSelect = sndBack = sndInvalid = nullptr;

  ext2dir[Parser::EXT_SCRIPT] = "scripts";
  ext2dir[Parser::EXT_IMAGE] = "images";
  ext2dir[Parser::EXT_SOUND] = "sounds";
  ext2dir[Parser::EXT_MUSIC] = "music";
  ext2dir[Parser::EXT_FONT] = "fonts";
  ext2dir[Parser::EXT_VIDEO] = "videos";
  ext2dir[Parser::EXT_TEXT] = "texts";
}

scene::Scene::~Scene() {
  for (const auto *item : deleteFontVector) {
    delete item;
  }
  for (const auto *item : deleteSoundVector) {
    delete item;
  }
  // delete video;
}

void scene::Scene::init() {
  initialized = true;
  // Load the scene data from the file
  bool jsonSuccess = false;
  auto j_obj =
      fileIO::readJSON(util::getPath("scenes/" + name + '/' + name + ".json"));
  if (j_obj.is_object()) {
    try {
      parseJSON(j_obj);
      jsonSuccess = true;
    }
    catch (const nlohmann::detail::out_of_range &e) {
      error::error(name + ".json error: " + e.what());
      images.clear();
    }
  }
  if (!jsonSuccess) {
    error::error("Cannot read " + name + ".json. Falling back to " + name +
                 ".ubu");
    parseFile("scenes/" + name + ".ubu");
  }
}

void scene::Scene::think() {
  if (!initialized) {
    init();
  }

  int sceneIndex = getSceneIndex();
  if (!bgmPlaying &&
#ifndef NO_NETWORK
      sceneIndex != SCENE_NETPLAY &&
#endif
      sceneIndex != SCENE_FIGHT) {
    if (bgm.exists()) {
      bgm.play();
    }
    bgmPlaying = true;
  }
  std::for_each(images.begin(), images.end(),
                [](SceneImage &si) { si.think(); });
  // if(video) video->think();

  // Fade timer
  float &fade = getrFade();
  if (fade) {
    // if(fadeIn && scene == SCENE_VERSUS) {
    //	fade = 0.0f;
    //} else {
    if (sceneIndex == SCENE_FIGHT) {
      fade -= 0.02;
    }
    else {
      fade -= 0.1f;
    }
    if (fade <= 0.0f) {
      if (isFadeIn()) {
        fade = 0.0f;
      }
      else {
        fade = 1.0f;
        setFadeIn(true);

        // Are we quitting?
        int sceneNew = getSceneNewIndex();
        if (sceneNew == SCENE_QUIT) {
          exit(0);
        }
        setIMSceneIndex(sceneNew);

        audio::Music::stop();

        const Image *imgLoading = getLoadingImage();
        if (!SCENE->initialized) {
          // Loading graphic
          imgLoading->draw<renderer::Texture2DRenderer>(0, 0);
          sys::refresh();
          SCENE->init();
        }
        if (getSceneIndex() == SCENE_FIGHT && !STAGE->isInitialized()) {
          // Loading graphic
          imgLoading->draw<renderer::Texture2DRenderer>(0, 0);
          sys::refresh();
          STAGE->init();
        }
        SCENE->reset();
      }
    }

    // Always disable controls during fades
    Fight::getrPlayerAt(0).setFrameInput(0);
    //}
  }
}

void scene::Scene::reset() {
  std::for_each(images.begin(), images.end(),
                [](SceneImage &si) { si.reset(); });
  // if(video) video->reset();
  bgmPlaying = false;
}

void scene::Scene::draw() const {
  // if(video) video->draw(0, 0);
  std::for_each(images.cbegin(), images.cend(),
                [](const SceneImage &si) { si.draw(false); });
}

template <>
Font *scene::Scene::getResourceT<Font>(const std::string &resource) {
  if (resource.front() == '*') {
    return resource_manager::getResource<Font>(
        resource.substr(1, std::string::npos));
  }
  else {
    Font *res = new Font;
    res->createFromFile(getResource(resource, Parser::EXT_FONT));
    deleteFontVector.push_back(res);
    return res;
  }
}

template <>
audio::Sound *
scene::Scene::getResourceT<audio::Sound>(const std::string &resource) {
  if (resource.front() == '*') {
    return resource_manager::getResource<audio::Sound>(
        resource.substr(1, std::string::npos));
  }
  else {
    audio::Sound *res = new audio::Sound;
    res->createFromFile(getResource(resource, Parser::EXT_SOUND));
    deleteSoundVector.push_back(res);
    return res;
  }
}

void scene::Scene::parseLine(Parser &parser) {
  int argc = parser.getArgC();
  if (parser.is("IMAGE", 3)) {
    float x = parser.getArgFloat(2);
    float y = parser.getArgFloat(3);
    Image::Render render = Image::Render::NORMAL;
    float xvel = 0.0f;
    float yvel = 0.0f;
    bool wrap = false;
    if (argc > 4) {
      std::string szRender = parser.getArg(4);
      if (!szRender.compare("additive")) {
        render = Image::Render::ADDITIVE;
      }
      else if (!szRender.compare("subtractive")) {
        render = Image::Render::SUBTRACTIVE;
      }
      else if (!szRender.compare("multiply")) {
        render = Image::Render::MULTIPLY;
      }
      if (argc > 5) {
        xvel = parser.getArgFloat(5);
        if (argc > 6) {
          yvel = parser.getArgFloat(6);
          if (argc > 7) {
            wrap = parser.getArgBool(7, false);
          }
        }
      }
    }

    // Add a new image
    Image imgData;
    imgData.createFromFile(getResource(parser.getArg(1), Parser::EXT_IMAGE));
    if (!imgData.exists()) {
      return;
    }
    images.emplace_back(imgData, x, y, 1.0f, render, xvel, yvel, wrap, 0);
  }
  else if (parser.is("BGM", 1)) {
    if (argc > 2) {
      std::string intro = getResource(parser.getArg(1), Parser::EXT_MUSIC);
      std::string loop = getResource(parser.getArg(2), Parser::EXT_MUSIC);
      bgm.createFromFile(intro, loop);
    }
    else {
      bgm.createFromFile("", getResource(parser.getArg(1), Parser::EXT_MUSIC));
    }
  }
  else if (parser.is("SOUND", 4)) {
    sndMenu = getResourceT<audio::Sound>(parser.getArg(1));
    sndSelect = getResourceT<audio::Sound>(parser.getArg(2));
    sndBack = getResourceT<audio::Sound>(parser.getArg(3));
    sndInvalid = getResourceT<audio::Sound>(parser.getArg(4));
  }
  else if (parser.is("VIDEO", 1)) {
    // getResource(parser.getArg(1), EXT_VIDEO);
  }
}

void scene::Scene::parseFile(std::string szFileName) {
  Parser parser;
  std::string path = util::getPath(szFileName);
  if (!parser.open(path)) {
    error::die("Cannot parse file \"" + path + "\"");
  }

  // Get all the data
  while (parser.parseLine()) {
    // Parse it
    parseLine(parser);
  }
}

void scene::Scene::parseJSON(const nlohmann::ordered_json &j_obj) {
  if (j_obj.contains("images")) {
    for (const auto &image : j_obj["images"]) {
      // Add a new image
      Image imgData;
      imgData.createFromFile(getResource(image["image"], Parser::EXT_IMAGE));
      if (imgData.exists()) {
        float x = image.at("pos").at("x");
        float y = image.at("pos").at("y");
        std::string szRender = image.value("renderType", "normal");
        Image::Render render = Image::Render::NORMAL;
        if (!szRender.compare("additive")) {
          render = Image::Render::ADDITIVE;
        }
        else if (!szRender.compare("subtractive")) {
          render = Image::Render::SUBTRACTIVE;
        }
        else if (!szRender.compare("multiply")) {
          render = Image::Render::MULTIPLY;
        }
        float xvel = 0.0f;
        float yvel = 0.0f;
        if (image.contains("vel")) {
          xvel = image["vel"].value("x", 0.0f);
          yvel = image["vel"].value("y", 0.0f);
        }
        bool wrap = image.value("wrap", false);
        images.emplace_back(imgData, x, y, 1.0f, render, xvel, yvel, wrap, 0);
      }
    }
  }
  if (j_obj.contains("bgm")) {
    std::string loop = getResource(j_obj["bgm"].at("loop"), Parser::EXT_MUSIC);
    std::string intro = j_obj["bgm"].value("intro", "");
    if (intro.empty()) {
      bgm.createFromFile("", loop);
    }
    else {
      bgm.createFromFile(getResource(intro, Parser::EXT_MUSIC), loop);
    }
  }
  if (j_obj.contains("sound")) {
    sndMenu = getResourceT<audio::Sound>(j_obj["sound"].at("menu"));
    sndSelect = getResourceT<audio::Sound>(j_obj["sound"].at("select"));
    sndBack = getResourceT<audio::Sound>(j_obj["sound"].at("back"));
    sndInvalid = getResourceT<audio::Sound>(j_obj["sound"].at("invalid"));
  }
  if (j_obj.contains("video")) {
    //
  }
}

std::string scene::Scene::getResource(std::string resource,
                                      std::string extension) const {
  if (*resource.c_str() == '*') {
    return ext2dir.at(extension) + '/' + resource.substr(1, std::string::npos) +
           '.' + extension;
  }
  else {
    return "scenes/" + name + "/" + resource + "." + extension;
  }
}

// SCENE IMAGE

scene::SceneImage::SceneImage(Image &image_, float x_, float y_,
                              float parallax_, Image::Render render_,
                              float xvel_, float yvel_, bool wrap_,
                              int round_) {
  image = std::move(image_);
  x = x_;
  y = y_;
  parallax = parallax_;
  xOrig = x_;
  yOrig = y_;
  render = render_;
  xvel = xvel_;
  yvel = yvel_;
  wrap = wrap_;
  round = round_;

  if (wrap) {
    unsigned int imageW = image.getW();
    unsigned int imageH = image.getH();
    while (x < 0.0f - imageW) {
      x += imageW;
    }
    while (y < 0.0f - imageH) {
      y += imageH;
    }
    while (x >= 0.0f) {
      x -= imageW;
    }
    while (y >= 0.0f) {
      y -= imageH;
    }
  }
}

scene::SceneImage::~SceneImage() {}

void scene::SceneImage::think() {
  x += xvel;
  y += yvel;

  // Wrap the wrapping images
  if (wrap) {
    unsigned int imageW = image.getW();
    unsigned int imageH = image.getH();
    while (x < 0.0f - imageW) {
      x += imageW;
    }
    while (y < 0.0f - imageH) {
      y += imageH;
    }
    while (x >= 0.0f) {
      x -= imageW;
    }
    while (y >= 0.0f) {
      y -= imageH;
    }
  }
}

void scene::SceneImage::reset() {
  x = xOrig;
  y = yOrig;
}

void scene::SceneImage::draw(bool _stage) const {
  unsigned int imageW = image.getW();
  unsigned int imageH = image.getH();
  if (image.exists()) {
    // Draw the image differently if wrapping
    if (wrap) {
      // How many of these are needed to fill the screen?
      int xCount = sys::WINDOW_WIDTH / imageW + 2;
      int yCount = sys::WINDOW_HEIGHT / imageH + 2;

      for (int i = 0; i < xCount; i++) {
        for (int j = 0; j < yCount; j++) {
          graphics::setRender(render);
          image.draw<renderer::Texture2DRenderer>((int)x + i * imageW,
                                                  (int)y + j * imageH);
        }
      }
    }
    else {
      graphics::setRender(render);
      if (_stage) {
        if (!round || round - 1 == FIGHT->getRound()) {
          const util::Vector &cameraPos = Fight::getrCameraPos();
          image.draw<renderer::Texture2DRenderer>(
              x - imageW / 2 + sys::WINDOW_WIDTH / 2 - cameraPos.x * parallax,
              (sys::WINDOW_HEIGHT - y) - imageH + cameraPos.y * parallax);
        }
      }
      else {
        image.draw<renderer::Texture2DRenderer>(x, y);
      }
    }
  }
}
