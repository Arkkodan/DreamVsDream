#include "title.h"

#include "fight.h"
#include "scene.h"

#include "../../fileIO/json.h"
#include "../../util/fileIO.h"
#include "../../util/rng.h"
#include "../app.h"
#include "../error.h"
#include "../menu/button.h"
#include "../player.h"

#include <memory>

const std::array<std::string, scene::Title::TITLE_MAIN_MAX>
    scene::Title::submenuMainStrings = {"Single-Player", "Multiplayer",
                                        "Options", "Quit"};
const std::array<std::string, scene::Title::TITLE_SINGLEPLAYER_MAX>
    scene::Title::submenuSPStrings = {"Arcade", "Story", "Versus CPU",
                                      "Survival", "Training"};
const std::array<std::string, scene::Title::TITLE_MULTIPLAYER_MAX>
    scene::Title::submenuMPStrings = {"Versus Player", "Tag Team", "2v2 Team",
                                      "Netplay"};

scene::Title::Title() : Scene("title") {
  menuX = menuY = 0;
  menuXOffset = 0;
  iR = iG = iB = aR = aG = aB = 255;
  aXOffset = 0;

  nThemes = 0;
  menuFont = nullptr;
}

scene::Title::~Title() {}

void scene::Title::init() {
  Scene::init();

  // Parse a random theme
  if (nThemes) {
    // util::roll a weighted random number, favoring the "normal" title
    int i = util::roll(nThemes + 5);
    i -= 5;
    if (i < 0) {
      i = 0;
    }

    bool jsonSuccess = false;
    auto j_obj =
        fileIO::readJSON(util::getPath("scenes/title/" + themes[i] + ".json"));
    if (!j_obj.is_null() && j_obj.is_object()) {
      try {
        parseJSON(j_obj);
        jsonSuccess = true;
      }
      catch (const nlohmann::detail::out_of_range &e) {
        error::error(themes[i] + ".json error: " + e.what());
      }
    }
    if (!jsonSuccess) {
      error::error("Cannot read " + themes[i] + ".json. Falling back to " +
                   themes[i] + ".ubu");
      parseFile(getResource(themes[i], Parser::EXT_SCRIPT));
    }
  }

  uint16_t up = game::INPUT_UP;
  uint16_t down = game::INPUT_DOWN;

  if (menuXOffset < 0) {
    up |= game::INPUT_RIGHT;
    down |= game::INPUT_LEFT;
  }
  else {
    up |= game::INPUT_LEFT;
    down |= game::INPUT_RIGHT;
  }

  { // Single-Player
    std::vector<std::unique_ptr<menu::IMenuElement>> elements;
    for (int i = 0; i < TITLE_SINGLEPLAYER_MAX; i++) {
      auto textButton = std::make_unique<menu::TextButtonA>();
      textButton->setFont(menuFont);
      textButton->setPos(menuX + i * menuXOffset,
                         menuY + i * menuFont->getcImage()->getH(), aXOffset);
      textButton->setColorActive(aR, aG, aB);
      textButton->setColorInctive(iR, iG, iB);
      textButton->setText(submenuSPStrings[i]);
      textButton->setSelectSound(sndSelect);
      textButton->setInvalidSound(sndInvalid);

      switch (i) {
      case TITLE_SINGLEPLAYER_TRAINING:
        textButton->setAction([this]() {
          FIGHT->setGameType(Fight::GAMETYPE_TRAINING);
          setScene(SCENE_SELECT);
        });
        break;
      default:
        break;
      }

      elements.emplace_back(std::move(textButton));
    }
    submenuSP.setElements(std::move(elements));
    submenuSP.setInputMask(down, up);
    submenuSP.setMenuSound(sndMenu);
  }
  { // Multiplayer
    std::vector<std::unique_ptr<menu::IMenuElement>> elements;
    for (int i = 0; i < TITLE_MULTIPLAYER_MAX; i++) {
      auto textButton = std::make_unique<menu::TextButtonA>();
      textButton->setFont(menuFont);
      textButton->setPos(menuX + i * menuXOffset,
                         menuY + i * menuFont->getcImage()->getH(), aXOffset);
      textButton->setColorActive(aR, aG, aB);
      textButton->setColorInctive(iR, iG, iB);
      textButton->setText(submenuMPStrings[i]);
      textButton->setSelectSound(sndSelect);
      textButton->setInvalidSound(sndInvalid);

      switch (i) {
      case TITLE_MULTIPLAYER_VS_PLAYER:
        textButton->setAction([this]() {
          FIGHT->setGameType(Fight::GAMETYPE_VERSUS);
          setScene(SCENE_SELECT);
        });
        break;
      case TITLE_MULTIPLAYER_NETPLAY:
        textButton->setAction([this]() { setScene(SCENE_NETPLAY); });
        break;
      default:
        break;
      }

      elements.emplace_back(std::move(textButton));
    }
    submenuMP.setElements(std::move(elements));
    submenuMP.setInputMask(down, up);
    submenuMP.setMenuSound(sndMenu);
  }
  { // Main
    std::vector<std::unique_ptr<menu::IMenuElement>> elements;
    for (int i = 0; i < TITLE_MAIN_MAX; i++) {
      auto textButton = std::make_unique<menu::TextButtonA>();
      textButton->setFont(menuFont);
      textButton->setPos(menuX + i * menuXOffset,
                         menuY + i * menuFont->getcImage()->getH(), aXOffset);
      textButton->setColorActive(aR, aG, aB);
      textButton->setColorInctive(iR, iG, iB);
      textButton->setText(submenuMainStrings[i]);
      textButton->setSelectSound(sndSelect);
      textButton->setInvalidSound(sndInvalid);

      switch (i) {
      case TITLE_MAIN_SINGLEPLAYER:
        textButton->setAction(
            [this]() { submenuMain.pushSubmenu(&submenuSP); });
        break;
      case TITLE_MAIN_MULTIPLAYER:
        textButton->setAction(
            [this]() { submenuMain.pushSubmenu(&submenuMP); });
        break;
      case TITLE_MAIN_OPTIONS:
        textButton->setAction([]() { setScene(SCENE_OPTIONS); });
        break;
      case TITLE_MAIN_QUIT:
        textButton->setAction([]() {
          setScene(SCENE_QUIT);
          app::quit();
        });
        break;
      default:
        break;
      }

      elements.emplace_back(std::move(textButton));
    }
    submenuMain.setElements(std::move(elements));
    submenuMain.setInputMask(down, up);
    submenuMain.setAction([]() {
      // Quit
      app::quit();
      setScene(SCENE_QUIT);
    });
    submenuMain.setMenuSound(sndMenu);
    submenuMain.setBackSound(sndBack);
  }
}

void scene::Title::think() {
  Scene::think();

  submenuMain.think();

  for (int i = 0; i < 2; i++) {
    submenuMain.doInput(Fight::getrPlayerAt(i).getFrameInput(), i);
  }
}

void scene::Title::reset() {
  Scene::reset();
  submenuMain.reset();
  submenuSP.reset();
  submenuMP.reset();
}

void scene::Title::draw() const {
  Scene::draw();

  submenuMain.draw();
}

void scene::Title::parseLine(Parser &parser) {
  int argc = parser.getArgC();
  if (parser.is("MENU", 3)) {
    // Font
    menuFont = getResourceT<Font>(parser.getArg(1));

    menuX = parser.getArgInt(2);
    menuY = parser.getArgInt(3);

    if (argc > 4) {
      menuXOffset = parser.getArgInt(4);
    }
  }
  else if (parser.is("INACTIVE", 3)) {
    iR = parser.getArgInt(1);
    iG = parser.getArgInt(2);
    iB = parser.getArgInt(3);
  }
  else if (parser.is("ACTIVE", 3)) {
    aR = parser.getArgInt(1);
    aG = parser.getArgInt(2);
    aB = parser.getArgInt(3);

    if (argc >= 4) {
      aXOffset = parser.getArgInt(4);
    }
  }
  else if (parser.is("THEMES", 1)) {
    nThemes = 0;
    themes.resize(parser.getArgInt(1));
  }
  else if (parser.is("THEME", 1)) {
    themes[nThemes++] = parser.getArg(1);
  }
  else {
    Scene::parseLine(parser);
  }
}

void scene::Title::parseJSON(const nlohmann::ordered_json &j_obj) {
  if (j_obj.contains("menu")) {
    menuFont = getResourceT<Font>(j_obj["menu"].at("font"));
    menuX = j_obj["menu"].at("pos").at("x");
    menuY = j_obj["menu"].at("pos").at("y");
    menuXOffset = j_obj["menu"].value("dx", 0);
  }
  if (j_obj.contains("inactive")) {
    iR = j_obj["inactive"].at("r");
    iG = j_obj["inactive"].at("g");
    iB = j_obj["inactive"].at("b");
  }
  if (j_obj.contains("active")) {
    aR = j_obj["active"].at("r");
    aG = j_obj["active"].at("g");
    aB = j_obj["active"].at("b");
    aXOffset = j_obj["active"].value("dx", 0);
  }
  if (j_obj.contains("themes")) {
    themes.clear();
    for (const auto &theme : j_obj["themes"]) {
      themes.push_back(theme);
    }
    nThemes = themes.size();
  }
  Scene::parseJSON(j_obj);
}
