#include "title.h"

#include "fight.h"
#include "scene.h"

#include "../../fileIO/json.h"
#include "../../util/fileIO.h"
#include "../../util/rng.h"
#include "../error.h"
#include "../player.h"

std::vector<std::string> scene::Title::menuChoicesMain = {
    "Arcade",  "Story", "Versus", "Survival", "Training",
#ifndef NO_NETWORK
    "Netplay",
#endif
    "Options", "Quit",
};

std::vector<std::string> scene::Title::menuChoicesVersus = {
    "Versus Player", "Versus CPU", "Tag Team", "2v2 Team", "Return",
};

const std::array<std::vector<std::string> *, scene::Title::TM_MAX>
    scene::Title::menuChoices = {
        &menuChoicesMain,
        &menuChoicesVersus,
};

const std::array<int, scene::Title::TM_MAX> scene::Title::menuChoicesMax = {
    CHOICE_MAX,
    CHOICE_VS_MAX,
};

scene::Title::Title() : Scene("title") {
  menuX = menuY = 0;
  menuXOffset = 0;
  iR = iG = iB = aR = aG = aB = 255;
  aXOffset = 0;

  choice = choiceLast = choiceTimer = 0;
  submenu = 0;

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
}

void scene::Title::think() {
  Scene::think();

  if (choiceTimer) {
    if (choiceTimer == 1 || choiceTimer == -1) {
      choiceTimer = 0;
    }
    else {
      choiceTimer /= 2;
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

  if (input(up)) {
    sndMenu->play();

    choiceTimer = aXOffset;
    choiceLast = choice;
    if (choice == 0) {
      choice = menuChoicesMax[submenu] - 1;
    }
    else {
      choice--;
    }
  }
  else if (input(down)) {
    sndMenu->play();

    choiceTimer = aXOffset;
    choiceLast = choice;
    if (choice >= menuChoicesMax[submenu] - 1) {
      choice = 0;
    }
    else {
      choice++;
    }
  }
  else if (input(game::INPUT_A)) {
    // Enter the new menu
    switch (submenu) {
    case TM_MAIN:
      switch (choice) {
      case CHOICE_VERSUS:
        sndSelect->play();
        choice = 0;
        choiceLast = 0;
        choiceTimer = 0;
        submenu = TM_VERSUS;
        break;
      case CHOICE_TRAINING:
        sndSelect->play();
        FIGHT->gametype = Fight::GAMETYPE_TRAINING;
        setScene(SCENE_SELECT);
        break;
      default:
        // if(sndSelect) sndSelect->play();
        // setScene(SCENE_SELECT);
        sndInvalid->play();
        break;
#ifndef NO_NETWORK
      case CHOICE_NETPLAY:
        sndSelect->play();
        setScene(SCENE_NETPLAY);
        break;
#endif
      case CHOICE_OPTIONS:
        sndSelect->play();
        setScene(SCENE_OPTIONS);
        break;
      case CHOICE_QUIT:
        setScene(SCENE_QUIT);
        break;
      }
      break;

    case TM_VERSUS:
      switch (choice) {
      case CHOICE_VS_PLR:
        sndSelect->play();
        FIGHT->gametype = Fight::GAMETYPE_VERSUS;
        setScene(SCENE_SELECT);
        break;
      default:
        // if(sndSelect) sndSelect->play();
        // setScene(SCENE_SELECT);
        sndInvalid->play();
        break;
      case CHOICE_VS_RETURN:
        sndBack->play();
        choice = CHOICE_VERSUS;
        choiceLast = choice;
        choiceTimer = 0;
        submenu = TM_MAIN;
        break;
      }
      break;
    }
  }
  else if (input(game::INPUT_B)) {
    if (submenu == TM_MAIN) {
      // Quit
      setScene(SCENE_QUIT);
    }
    else if (submenu == TM_VERSUS) {
      // Return
      sndBack->play();
      choice = CHOICE_VERSUS;
      choiceLast = choice;
      choiceTimer = 0;
      submenu = TM_MAIN;
    }
  }
}

void scene::Title::reset() { Scene::reset(); }

void scene::Title::draw() const {
  Scene::draw();

  if (menuFont->exists()) {
    for (int i = 0; i < menuChoicesMax[submenu]; i++) {
      int gray = 2;
      if (submenu == TM_MAIN) {
        switch (i) {
        case CHOICE_VERSUS:
        case CHOICE_TRAINING:
        case CHOICE_OPTIONS:
#ifndef NO_NETWORK
        case CHOICE_NETPLAY:
#endif
        case CHOICE_QUIT:
          gray = 1;
          break;
        }
      }
      else if (submenu == TM_VERSUS) {
        switch (i) {
        case CHOICE_VS_PLR:
        case CHOICE_VS_RETURN:
          gray = 1;
          break;
        }
      }

      unsigned int fontH = menuFont->img.getH();
      if (i == choice) {
        menuFont->drawText(menuX + i * menuXOffset + (aXOffset - choiceTimer),
                           menuY + fontH * i, menuChoices[submenu]->at(i),
                           aR / gray, aG / gray, aB / gray, 1.0f);
      }
      else if (i == choiceLast) {
        menuFont->drawText(menuX + i * menuXOffset + choiceTimer,
                           menuY + fontH * i, menuChoices[submenu]->at(i),
                           iR / gray, iG / gray, iB / gray, 1.0f);
      }
      else {
        menuFont->drawText(menuX + i * menuXOffset, menuY + fontH * i,
                           menuChoices[submenu]->at(i), iR / gray, iG / gray,
                           iB / gray, 1.0f);
      }
    }
  }
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
