#include "options.h"

#include "fight.h"
#include "scene.h"

#include "../../fileIO/json.h"
#include "../../util/fileIO.h"
#include "../../util/rng.h"
#include "../error.h"
#include "../graphics.h"
#include "../player.h"
#include "../resource_manager.h"
#include "../shader_renderer/texture2D_renderer.h"

#include <cstring>

int scene::Options::optionDifficulty = 3;
int scene::Options::optionWins = 2;
int scene::Options::optionTime = 99;
int scene::Options::optionSfxVolume = 100;
int scene::Options::optionMusVolume = 100;
int scene::Options::optionVoiceVolume = 100;
bool scene::Options::optionEpilepsy = false;

scene::Options::Options() : Scene("options") {
  cursor = cursorLast = cursorTimer = 0;
  madoPos = 0;
  madoFrame = 1;
  madoDir = 2;
  madoWakeTimer = 0;
  themes.clear();
  nThemes = 0;
  iR = iG = iB = aR = aG = aB = 0;
  aXOffset = 0;
  madoSfxStep = madoSfxPinch = dame = muri = nullptr;
  menuFont = nullptr;
}

scene::Options::~Options() {}

void scene::Options::think() {
  Scene::think();

  // Cursor stuff
  if (cursorTimer) {
    if (cursorTimer == 1 || cursorTimer == -1) {
      cursorTimer = 0;
    }
    else {
      cursorTimer /= 2;
    }
  }

  // Move/animate Madotsuki
  if (madoWakeTimer) {
    if (madoWakeTimer == 1) {
      madoWakeTimer = -1;
      setScene(SCENE_TITLE);
    }
    else {
      madoWakeTimer--;
      if (madoWakeTimer == 36) {
        madoFrame++;
      }
      else if (madoWakeTimer == 32) {
        madoSfxPinch->play();
        madoFrame++;
      }
    }
  }
  else {
    bool stopped = false;
    if (madoPos < cursor * 32) {
      // Move down
      madoDir = 2;
      madoPos++;
      if (madoPos % 8 == 0) {
        madoFrame++;
        if (madoFrame >= 4) {
          madoFrame = 0;
        }
        if (madoFrame % 2 == 1) {
          madoSfxStep->play();
        }
      }
    }
    else if (madoPos > cursor * 32) {
      // Move up
      madoDir = 0;
      madoPos--;
      if (madoPos % 8 == 0) {
        madoFrame++;
        if (madoFrame >= 4) {
          madoFrame = 0;
        }
        if (madoFrame % 2 == 1) {
          madoSfxStep->play();
        }
      }
    }
    else {
      madoFrame = 1;
      stopped = true;
    }

    if (input(game::INPUT_UP | game::INPUT_DOWN)) {
      cursorTimer = aXOffset;
      cursorLast = cursor;
    }

    if (input(game::INPUT_UP)) {
      sndMenu->play();
      if (cursor) {
        cursor--;
      }
      else {
        cursor = OPTION_MAX - 1;
      }
    }
    else if (input(game::INPUT_DOWN)) {
      sndMenu->play();
      if (cursor < OPTION_MAX - 1) {
        cursor++;
      }
      else {
        cursor = 0;
      }
    }
    if (stopped) {
      if (input(game::INPUT_LEFT)) {
        madoDir = 3;
      }
      else if (input(game::INPUT_RIGHT)) {
        madoDir = 1;
      }
    }

    // Change option
    if (input(game::INPUT_LEFT) || input(game::INPUT_RIGHT)) {
      if (cursor != OPTION_VOICE_VOLUME) {
        sndMenu->play();
      }

      switch (cursor) {
      case OPTION_DIFFICULTY:
        if (input(game::INPUT_LEFT)) {
          if (optionDifficulty > 1) {
            optionDifficulty--;
          }
        }
        else {
          if (optionDifficulty < 5) {
            optionDifficulty++;
          }
        }
        break;

      case OPTION_WINS:
        if (input(game::INPUT_LEFT)) {
          if (optionWins > 1) {
            optionWins--;
          }
        }
        else {
          if (optionWins < 3) {
            optionWins++;
          }
        }
        break;

      case OPTION_TIME:
        if (input(game::INPUT_LEFT)) {
          if (optionTime == 0) {
            optionTime = 99;
          }
          else if (optionTime == 99) {
            optionTime = 60;
          }
        }
        else {
          if (optionTime == 60) {
            optionTime = 99;
          }
          else if (optionTime == 99) {
            optionTime = 0;
          }
        }
        break;

      case OPTION_SFX_VOLUME:
        if (input(game::INPUT_LEFT)) {
          if (optionSfxVolume > 0) {
            optionSfxVolume -= 10;
          }
        }
        else {
          if (optionSfxVolume < 100) {
            optionSfxVolume += 10;
          }
        }
        break;

      case OPTION_MUS_VOLUME:
        if (input(game::INPUT_LEFT)) {
          if (optionMusVolume > 0) {
            optionMusVolume -= 10;
          }
        }
        else {
          if (optionMusVolume < 100) {
            optionMusVolume += 10;
          }
        }
        break;

      case OPTION_VOICE_VOLUME:
        if (input(game::INPUT_LEFT)) {
          if (optionVoiceVolume > 0) {
            optionVoiceVolume -= 10;
          }
          Fight::madotsuki.speaker.play(dame);
        }
        else {
          if (optionVoiceVolume < 100) {
            optionVoiceVolume += 10;
          }
          Fight::madotsuki.speaker.play(muri);
        }
        break;

      case OPTION_EPILEPSY:
        if (input(game::INPUT_LEFT | game::INPUT_RIGHT)) {
          optionEpilepsy = !optionEpilepsy;
        }
        break;
      }
    }

    if (input(game::INPUT_A)) {
      if (cursor == OPTION_CREDITS) {
        sndSelect->play();
        setScene(SCENE_CREDITS);
      }
    }

    // Pinch thyself awake, Madotsuki
    if (input(game::INPUT_B)) {
      madoDir = 4;
      madoFrame = 0;
      madoWakeTimer = 40;
    }
  }
}

void scene::Options::reset() {
  Scene::reset();

  cursor = cursorLast = cursorTimer = 0;
  madoPos = 0;
  madoFrame = 1;
  madoDir = 2;
  madoWakeTimer = 0;
}

void scene::Options::draw() const {
  Scene::draw();

  // Draw the menu options
  if (menuFont->exists()) {
    for (int i = 0; i < OPTION_MAX; i++) {
      char buff[80];
      switch (i) {
      case OPTION_DIFFICULTY:
        sprintf(buff, "Difficulty:\t%d", optionDifficulty);
        break;

      case OPTION_WINS:
        sprintf(buff, "Wins:\t%d", optionWins);
        break;

      case OPTION_TIME:
        if (optionTime) {
          sprintf(buff, "Time:\t%d", optionTime);
        }
        else {
          sprintf(buff, "Time:\tUnlimited");
        }
        break;

      case OPTION_SFX_VOLUME:
        sprintf(buff, "Sound Volume:\t%d%%", optionSfxVolume);
        break;

      case OPTION_MUS_VOLUME:
        sprintf(buff, "Music Volume:\t%d%%", optionMusVolume);
        break;

      case OPTION_VOICE_VOLUME:
        sprintf(buff, "Voice Volume:\t%d%%", optionVoiceVolume);
        break;

      case OPTION_EPILEPSY:
        if (optionEpilepsy) {
          strcpy(buff, "Photosensitivity Mode:\tOn");
        }
        else {
          strcpy(buff, "Photosensitivity Mode:\tOff");
        }
        break;

      case OPTION_CREDITS:
        strcpy(buff, "Credits");
        break;
      }

      if (i == cursor) {
        menuFont->drawText(64 + (aXOffset - cursorTimer), 64 + i * 32, buff, aR,
                           aG, aB);
      }
      else if (i == cursorLast) {
        menuFont->drawText(64 + cursorTimer, 64 + i * 32, buff, iR, iG, iB);
      }
      else {
        menuFont->drawText(64, 64 + i * 32, buff, iR, iG, iB);
      }
    }
  }

  int xoff = madoFrame * 16;
  if (madoFrame == 3) {
    xoff = 16;
  }
  graphics::setRect(xoff, madoDir * 32, 16, 32);
  madoImg.draw<renderer::Texture2DRenderer>(32, 64 + madoPos);
}

void scene::Options::init() {
  Scene::init();

  // Parse a random theme
  if (nThemes) {
    int i = util::roll(nThemes);
    bool jsonSuccess = false;
    auto j_obj = fileIO::readJSON(
        util::getPath("scenes/options/" + themes[i] + ".json"));
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

void scene::Options::parseLine(Parser &parser) {
  int argc = parser.getArgC();
  if (parser.is("FONT", 1)) {
    // The font
    menuFont = getResourceT<Font>(parser.getArg(1));
  }
  else if (parser.is("MADOTSUKI", 3)) {
    // Madotsuki sprites/sounds
    madoImg.createFromFile(getResource(parser.getArg(1), Parser::EXT_IMAGE));
    madoSfxStep = getResourceT<audio::Sound>(parser.getArg(2));
    madoSfxPinch = getResourceT<audio::Sound>(parser.getArg(3));
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

    if (argc > 4) {
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
  else if (parser.is("VOICES", 2)) {
    dame = getResourceT<audio::Voice>(parser.getArg(1));
    muri = getResourceT<audio::Voice>(parser.getArg(2));
  }
  else {
    Scene::parseLine(parser);
  }
}

void scene::Options::parseJSON(const nlohmann::ordered_json &j_obj) {
  if (j_obj.contains("font")) {
    menuFont = getResourceT<Font>(j_obj["font"]);
  }
  if (j_obj.contains("madotsuki")) {
    madoImg.createFromFile(
        getResource(j_obj["madotsuki"].at("image"), Parser::EXT_IMAGE));
    madoSfxStep =
        getResourceT<audio::Sound>(j_obj["madotsuki"].at("sfx").at("step"));
    madoSfxPinch =
        getResourceT<audio::Sound>(j_obj["madotsuki"].at("sfx").at("pinch"));
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
  if (j_obj.contains("voices")) {
    dame = getResourceT<audio::Voice>(j_obj["voices"].at("dame"));
    muri = getResourceT<audio::Voice>(j_obj["voices"].at("muri"));
  }
  Scene::parseJSON(j_obj);
}
