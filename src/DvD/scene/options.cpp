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

const std::array<std::string, scene::Options::OPTION_MAIN_MAX>
    scene::Options::submenuMainStrings = {"Game Options",  "Audio Options",
                                          "Video Options", "Credits",
                                          "Intro",         "Controls"};
const std::array<std::string, scene::Options::OPTION_GAME_MAX>
    scene::Options::submenuGameStrings = {"Difficulty:", "Wins:", "Time:"};
const std::array<std::string, scene::Options::OPTION_AUDIO_MAX>
    scene::Options::submenuAudioStrings = {
        "Sound Volume:", "Music Volume:", "Voice Volume:"};
const std::array<std::string, scene::Options::OPTION_VIDEO_MAX>
    scene::Options::submenuVideoStrings = {"Photosensitivity Mode:"};

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

  submenuMain.think();

  int cursor = submenuMain.getActiveSubmenuIndex();

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
      submenuMain.doInputP(Fight::getrPlayerAt(i).getFrameInput(), i);
    }

    // Update values
    const auto *e_difficulty = dynamic_cast<const menu::SelectorLR<int> *>(
        submenuGame.getcElementAt(OPTION_GAME_DIFFICULTY));
    if (e_difficulty) {
      optionDifficulty = e_difficulty->getValue();
    }
    const auto *e_wins = dynamic_cast<const menu::SelectorLR<int> *>(
        submenuGame.getcElementAt(OPTION_GAME_WINS));
    if (e_wins) {
      optionWins = e_wins->getValue();
    }
    const auto *e_time = dynamic_cast<const menu::SelectorLR<int> *>(
        submenuGame.getcElementAt(OPTION_GAME_TIME));
    if (e_time) {
      optionTime = e_time->getValue();
    }
    const auto *e_sfxVol = dynamic_cast<const menu::SelectorLR<int> *>(
        submenuAudio.getcElementAt(OPTION_AUDIO_SFX_VOLUME));
    if (e_sfxVol) {
      optionSfxVolume = e_sfxVol->getValue();
    }
    const auto *e_musVol = dynamic_cast<const menu::SelectorLR<int> *>(
        submenuAudio.getcElementAt(OPTION_AUDIO_MUS_VOLUME));
    if (e_musVol) {
      optionMusVolume = e_musVol->getValue();
    }
    const auto *e_voiceVol = dynamic_cast<const menu::SelectorLR<int> *>(
        submenuAudio.getcElementAt(OPTION_AUDIO_VOICE_VOLUME));
    if (e_voiceVol) {
      optionVoiceVolume = e_voiceVol->getValue();
    }
    const auto *e_epilepsy = dynamic_cast<const menu::SelectorLR<bool> *>(
        submenuVideo.getcElementAt(OPTION_VIDEO_EPILEPSY));
    if (e_epilepsy) {
      optionEpilepsy = e_epilepsy->getValue();
    }
  }
}

void scene::Options::reset() {
  Scene::reset();

  submenuMain.setIndex(0);
  submenuMain.reset();
  submenuGame.setIndex(0);
  submenuGame.reset();
  submenuAudio.setIndex(0);
  submenuAudio.reset();
  submenuVideo.setIndex(0);
  submenuVideo.reset();

  madoPos = 0;
  madoFrame = 1;
  madoDir = 2;
  madoWakeTimer = 0;
}

void scene::Options::draw() const {
  Scene::draw();

  submenuMain.draw();

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

  { // Game
    std::vector<std::unique_ptr<menu::IMenuElement>> elements;
    auto e_difficulty = std::make_unique<menu::TextSelectorLR<int>>();
    e_difficulty->setValuePairs({1, 2, 3, 4, 5}, {"1", "2", "3", "4", "5"});
    e_difficulty->setIndexByValue(optionDifficulty);
    elements.emplace_back(std::move(e_difficulty));
    auto e_wins = std::make_unique<menu::TextSelectorLR<int>>();
    e_wins->setValuePairs({1, 2, 3}, {"1", "2", "3"});
    e_wins->setIndexByValue(optionWins);
    elements.emplace_back(std::move(e_wins));
    auto e_time = std::make_unique<menu::TextSelectorLR<int>>();
    e_time->setValuePairs({60, 99, 0}, {"60", "99", "Unlimited"});
    e_time->setIndexByValue(optionTime);
    elements.emplace_back(std::move(e_time));
    for (int i = 0; i < OPTION_GAME_MAX; i++) {
      auto *element = elements[i].get();
      auto *textElement = dynamic_cast<menu::ITextMenuElement *>(element);
      if (textElement) {
        textElement->setFont(menuFont);
        textElement->setPos(64, 64 + i * menuFont->getcImage()->getH(),
                            aXOffset);
        textElement->setColorActive(aR, aG, aB);
        textElement->setColorInctive(iR, iG, iB);
      }
      auto *selectorInt = dynamic_cast<menu::TextSelectorLR<int> *>(element);
      if (selectorInt) {
        selectorInt->setMenuSounds(sndMenu);
        selectorInt->setLabel(submenuGameStrings[i]);
      }
      auto *selectorBool = dynamic_cast<menu::TextSelectorLR<bool> *>(element);
      if (selectorBool) {
        selectorBool->setMenuSounds(sndMenu);
        selectorBool->setLabel(submenuGameStrings[i]);
      }
      auto *button = dynamic_cast<menu::TextButtonA *>(element);
      if (button) {
        button->setSelectSound(sndSelect);
        button->setText(submenuGameStrings[i]);
      }
    }
    submenuGame.setElements(std::move(elements));
    submenuGame.setInputMask(game::INPUT_DOWN, game::INPUT_UP);
    submenuGame.setMenuSound(sndMenu);
  }
  { // Audio
    std::vector<std::unique_ptr<menu::IMenuElement>> elements;
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
    e_sfxVol->setValuePairs(volumeOptions, volumeStrings);
    e_sfxVol->setIndexByValue(optionSfxVolume);
    elements.emplace_back(std::move(e_sfxVol));
    auto e_musVol = std::make_unique<menu::TextSelectorLR<int>>();
    e_musVol->setValuePairs(volumeOptions, volumeStrings);
    e_musVol->setIndexByValue(optionMusVolume);
    elements.emplace_back(std::move(e_musVol));
    auto e_voiceVol = std::make_unique<menu::TextSelectorLR<int>>();
    e_voiceVol->setValuePairs(volumeOptions, volumeStrings);
    e_voiceVol->setIndexByValue(optionVoiceVolume);
    elements.emplace_back(std::move(e_voiceVol));
    for (int i = 0; i < OPTION_AUDIO_MAX; i++) {
      auto *element = elements[i].get();
      auto *textElement = dynamic_cast<menu::ITextMenuElement *>(element);
      if (textElement) {
        textElement->setFont(menuFont);
        textElement->setPos(64, 64 + i * menuFont->getcImage()->getH(),
                            aXOffset);
        textElement->setColorActive(aR, aG, aB);
        textElement->setColorInctive(iR, iG, iB);
      }
      auto *selectorInt = dynamic_cast<menu::TextSelectorLR<int> *>(element);
      if (selectorInt) {
        switch (i) {
        case OPTION_AUDIO_MUS_VOLUME:
          selectorInt->setMenuSounds(nullptr);
          break;
        case OPTION_AUDIO_VOICE_VOLUME:
          selectorInt->setMenuSounds(dame, muri, true);
          break;
        default:
          selectorInt->setMenuSounds(sndMenu);
          break;
        }
        selectorInt->setLabel(submenuAudioStrings[i]);
      }
      auto *selectorBool = dynamic_cast<menu::TextSelectorLR<bool> *>(element);
      if (selectorBool) {
        selectorBool->setMenuSounds(sndMenu);
        selectorBool->setLabel(submenuAudioStrings[i]);
      }
      auto *button = dynamic_cast<menu::TextButtonA *>(element);
      if (button) {
        button->setSelectSound(sndSelect);
        button->setText(submenuAudioStrings[i]);
      }
    }
    submenuAudio.setElements(std::move(elements));
    submenuAudio.setInputMask(game::INPUT_DOWN, game::INPUT_UP);
    submenuAudio.setMenuSound(sndMenu);
  }
  { // Video
    std::vector<std::unique_ptr<menu::IMenuElement>> elements;
    auto e_epilepsy = std::make_unique<menu::TextSelectorLR<bool>>();
    e_epilepsy->setValuePairs({true, false}, {"On", "Off"});
    e_epilepsy->setWrap(true);
    e_epilepsy->setIndexByValue(optionEpilepsy);
    elements.emplace_back(std::move(e_epilepsy));
    for (int i = 0; i < OPTION_VIDEO_MAX; i++) {
      auto *element = elements[i].get();
      auto *textElement = dynamic_cast<menu::ITextMenuElement *>(element);
      if (textElement) {
        textElement->setFont(menuFont);
        textElement->setPos(64, 64 + i * menuFont->getcImage()->getH(),
                            aXOffset);
        textElement->setColorActive(aR, aG, aB);
        textElement->setColorInctive(iR, iG, iB);
      }
      auto *selectorInt = dynamic_cast<menu::TextSelectorLR<int> *>(element);
      if (selectorInt) {
        selectorInt->setMenuSounds(sndMenu);
        selectorInt->setLabel(submenuVideoStrings[i]);
      }
      auto *selectorBool = dynamic_cast<menu::TextSelectorLR<bool> *>(element);
      if (selectorBool) {
        selectorBool->setMenuSounds(sndMenu);
        selectorBool->setLabel(submenuVideoStrings[i]);
      }
      auto *button = dynamic_cast<menu::TextButtonA *>(element);
      if (button) {
        button->setSelectSound(sndSelect);
        button->setText(submenuVideoStrings[i]);
      }
    }
    submenuVideo.setElements(std::move(elements));
    submenuVideo.setInputMask(game::INPUT_DOWN, game::INPUT_UP);
    submenuVideo.setMenuSound(sndMenu);
  }
  { // Main
    std::vector<std::unique_ptr<menu::IMenuElement>> elements;
    auto e_game = std::make_unique<menu::TextButtonA>();
    e_game->setAction([this]() { submenuMain.pushSubmenu(&submenuGame); });
    elements.emplace_back(std::move(e_game));
    auto e_audio = std::make_unique<menu::TextButtonA>();
    e_audio->setAction([this]() { submenuMain.pushSubmenu(&submenuAudio); });
    elements.emplace_back(std::move(e_audio));
    auto e_video = std::make_unique<menu::TextButtonA>();
    e_video->setAction([this]() { submenuMain.pushSubmenu(&submenuVideo); });
    elements.emplace_back(std::move(e_video));
    auto e_credits = std::make_unique<menu::TextButtonA>();
    e_credits->setAction([]() { setScene(SCENE_CREDITS); });
    elements.emplace_back(std::move(e_credits));
    auto e_intro = std::make_unique<menu::TextButtonA>();
    e_intro->setAction([]() { setScene(SCENE_INTRO); });
    elements.emplace_back(std::move(e_intro));
    auto e_controls = std::make_unique<menu::TextButtonA>();
    e_controls->setAction([]() { setScene(SCENE_CONTROLS); });
    elements.emplace_back(std::move(e_controls));

    for (int i = 0; i < OPTION_MAIN_MAX; i++) {
      auto *element = elements[i].get();
      auto *textElement = dynamic_cast<menu::ITextMenuElement *>(element);
      if (textElement) {
        textElement->setFont(menuFont);
        textElement->setPos(64, 64 + i * menuFont->getcImage()->getH(),
                            aXOffset);
        textElement->setColorActive(aR, aG, aB);
        textElement->setColorInctive(iR, iG, iB);
      }
      auto *selectorInt = dynamic_cast<menu::TextSelectorLR<int> *>(element);
      if (selectorInt) {
        selectorInt->setMenuSounds(sndMenu);
        selectorInt->setLabel(submenuMainStrings[i]);
      }
      auto *selectorBool = dynamic_cast<menu::TextSelectorLR<bool> *>(element);
      if (selectorBool) {
        selectorBool->setMenuSounds(sndMenu);
        selectorBool->setLabel(submenuMainStrings[i]);
      }
      auto *button = dynamic_cast<menu::TextButtonA *>(element);
      if (button) {
        button->setSelectSound(sndSelect);
        button->setText(submenuMainStrings[i]);
      }
    }
    submenuMain.setElements(std::move(elements));
    submenuMain.setAction([this]() {
      madoDir = 4;
      madoFrame = 0;
      madoWakeTimer = 40;
    });
    submenuMain.setInputMask(game::INPUT_DOWN, game::INPUT_UP);
    submenuMain.setMenuSound(sndMenu);
    submenuMain.setBackSound(sndBack);
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
