#include "options.h"

#include "fight.h"
#include "scene.h"

#include "../../fileIO/json.h"
#include "../../util/fileIO.h"
#include "../../util/rng.h"
#include "../error.h"
#include "../graphics.h"
#include "../menu/button.h"
#include "../menu/selector.h"
#include "../player.h"
#include "../resource_manager.h"
#include "../shader_renderer/texture2D_renderer.h"

#include <algorithm>
#include <cstring>
#include <memory>

int scene::Options::optionDifficulty = 3;
int scene::Options::optionWins = 2;
int scene::Options::optionTime = 99;
int scene::Options::optionSfxVolume = 100;
int scene::Options::optionMusVolume = 100;
int scene::Options::optionVoiceVolume = 100;
bool scene::Options::optionEpilepsy = false;

int scene::Options::getDifficulty() { return optionDifficulty; }
void scene::Options::setDifficulty(int difficulty) {
  optionDifficulty = difficulty;
}
int scene::Options::getWins() { return optionWins; }
void scene::Options::setWins(int wins) { optionWins = wins; }
int scene::Options::getTime() { return optionTime; }
void scene::Options::setTime(int time) { optionTime = time; }
int scene::Options::getSfxVolume() { return optionSfxVolume; }
void scene::Options::setSfxVolume(int sfxVolume) {
  optionSfxVolume = sfxVolume;
}
int scene::Options::getMusVolume() { return optionMusVolume; }
void scene::Options::setMusVolume(int musVolume) {
  optionMusVolume = musVolume;
}
int scene::Options::getVoiceVolume() { return optionVoiceVolume; }
void scene::Options::setVoiceVolume(int voiceVolume) {
  optionVoiceVolume = voiceVolume;
}
bool scene::Options::isEpilepsy() { return optionEpilepsy; }
void scene::Options::setEpilepsy(bool epilepsy) { optionEpilepsy = epilepsy; }

scene::Options::Options() : Scene("options") {
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

  submenu.think();

  int cursor = submenu.getIndex();

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

    if (stopped) {
      if (input(game::INPUT_LEFT)) {
        madoDir = 3;
      }
      else if (input(game::INPUT_RIGHT)) {
        madoDir = 1;
      }
    }

    for (int i = 0; i < 2; i++) {
      submenu.doInput(Fight::getrPlayerAt(i).getFrameInput(), i);
    }

    // Update values
    const auto *e_difficulty = dynamic_cast<const menu::SelectorLR<int> *>(
        submenu.getcElementAt(OPTION_DIFFICULTY));
    if (e_difficulty) {
      optionDifficulty = e_difficulty->getValue();
    }
    const auto *e_wins = dynamic_cast<const menu::SelectorLR<int> *>(
        submenu.getcElementAt(OPTION_WINS));
    if (e_wins) {
      optionWins = e_wins->getValue();
    }
    const auto *e_time = dynamic_cast<const menu::SelectorLR<int> *>(
        submenu.getcElementAt(OPTION_TIME));
    if (e_time) {
      optionTime = e_time->getValue();
    }
    const auto *e_sfxVol = dynamic_cast<const menu::SelectorLR<int> *>(
        submenu.getcElementAt(OPTION_SFX_VOLUME));
    if (e_sfxVol) {
      optionSfxVolume = e_sfxVol->getValue();
    }
    const auto *e_musVol = dynamic_cast<const menu::SelectorLR<int> *>(
        submenu.getcElementAt(OPTION_MUS_VOLUME));
    if (e_musVol) {
      optionMusVolume = e_musVol->getValue();
    }
    const auto *e_voiceVol = dynamic_cast<const menu::SelectorLR<int> *>(
        submenu.getcElementAt(OPTION_VOICE_VOLUME));
    if (e_voiceVol) {
      optionVoiceVolume = e_voiceVol->getValue();
    }
    const auto *e_epilepsy = dynamic_cast<const menu::SelectorLR<bool> *>(
        submenu.getcElementAt(OPTION_EPILEPSY));
    if (e_epilepsy) {
      optionEpilepsy = e_epilepsy->getValue();
    }
  }
}

void scene::Options::reset() {
  Scene::reset();

  submenu.setIndex(0);
  submenu.reset();

  madoPos = 0;
  madoFrame = 1;
  madoDir = 2;
  madoWakeTimer = 0;
}

void scene::Options::draw() const {
  Scene::draw();

  submenu.draw();

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

  {
    std::vector<std::unique_ptr<menu::IMenuElement>> elements;
    auto e_difficulty = std::make_unique<menu::TextSelectorLR<int>>();
    e_difficulty->setLabel("Difficulty:");
    e_difficulty->setValuePairs({1, 2, 3, 4, 5}, {"1", "2", "3", "4", "5"});
    e_difficulty->setIndexByValue(optionDifficulty);
    elements.emplace_back(std::move(e_difficulty));
    auto e_wins = std::make_unique<menu::TextSelectorLR<int>>();
    e_wins->setLabel("Wins:");
    e_wins->setValuePairs({1, 2, 3}, {"1", "2", "3"});
    e_wins->setIndexByValue(optionWins);
    elements.emplace_back(std::move(e_wins));
    auto e_time = std::make_unique<menu::TextSelectorLR<int>>();
    e_time->setLabel("Time:");
    e_time->setValuePairs({60, 99, 0}, {"60", "99", "Unlimited"});
    e_time->setIndexByValue(optionTime);
    elements.emplace_back(std::move(e_time));
    std::vector<int> volumeOptions(11);
    std::generate(volumeOptions.begin(), volumeOptions.end(),
                  [vol = -10]() mutable {
                    vol += 10;
                    return vol;
                  });
    std::vector<std::string> volumeStrings(11);
    std::transform(volumeOptions.cbegin(), volumeOptions.cend(),
                   volumeStrings.begin(),
                   [](int vol) { return std::to_string(vol) + '%'; });
    auto e_sfxVol = std::make_unique<menu::TextSelectorLR<int>>();
    e_sfxVol->setLabel("Sound Volume:");
    e_sfxVol->setValuePairs(volumeOptions, volumeStrings);
    e_sfxVol->setIndexByValue(optionSfxVolume);
    elements.emplace_back(std::move(e_sfxVol));
    auto e_musVol = std::make_unique<menu::TextSelectorLR<int>>();
    e_musVol->setLabel("Music Volume:");
    e_musVol->setValuePairs(volumeOptions, volumeStrings);
    e_musVol->setIndexByValue(optionMusVolume);
    elements.emplace_back(std::move(e_musVol));
    auto e_voiceVol = std::make_unique<menu::TextSelectorLR<int>>();
    e_voiceVol->setLabel("Voice Volume:");
    e_voiceVol->setValuePairs(volumeOptions, volumeStrings);
    e_voiceVol->setIndexByValue(optionVoiceVolume);
    elements.emplace_back(std::move(e_voiceVol));
    auto e_epilepsy = std::make_unique<menu::TextSelectorLR<bool>>();
    e_epilepsy->setLabel("Photosensitivity Mode:");
    e_epilepsy->setValuePairs({true, false}, {"On", "Off"});
    e_epilepsy->setWrap(true);
    e_epilepsy->setIndexByValue(optionEpilepsy);
    elements.emplace_back(std::move(e_epilepsy));
    auto e_credits = std::make_unique<menu::TextButtonA>();
    e_credits->setText("Credits");
    e_credits->setAction([]() { setScene(SCENE_CREDITS); });
    elements.emplace_back(std::move(e_credits));

    for (int i = 0, size = elements.size(); i < size; i++) {
      auto *textElement =
          dynamic_cast<menu::ITextMenuElement *>(elements[i].get());
      if (textElement) {
        textElement->setFont(menuFont);
        textElement->setPos(64, 64 + i * menuFont->getcImage()->getH(),
                            aXOffset);
        textElement->setColorActive(aR, aG, aB);
        textElement->setColorInctive(iR, iG, iB);
      }
      auto *selectorInt =
          dynamic_cast<menu::SelectorLR<int> *>(elements[i].get());
      if (selectorInt) {
        switch (i) {
        case OPTION_MUS_VOLUME:
          selectorInt->setMenuSounds(nullptr);
          break;
        case OPTION_VOICE_VOLUME:
          selectorInt->setMenuSounds(dame, muri, true);
          break;
        default:
          selectorInt->setMenuSounds(sndMenu);
          break;
        }
      }
      auto *selectorBool =
          dynamic_cast<menu::SelectorLR<bool> *>(elements[i].get());
      if (selectorBool) {
        selectorBool->setMenuSounds(sndMenu);
      }
      auto *button = dynamic_cast<menu::TextButtonA *>(elements[i].get());
      if (button) {
        button->setSelectSound(sndSelect);
      }
    }
    submenu.setElements(std::move(elements));
    submenu.setAction([this]() {
      madoDir = 4;
      madoFrame = 0;
      madoWakeTimer = 40;
    });
    submenu.setInputMask(game::INPUT_DOWN, game::INPUT_UP);
    submenu.setMenuSound(sndMenu);
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
