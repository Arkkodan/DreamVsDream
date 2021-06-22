#include "controls.h"

#include "scene.h"

#include "../app.h"
#include "../input.h"
#include "../menu/button.h"
#include "../sys.h"

#include <SDL_keyboard.h>

#include <fstream>
#include <memory>

namespace {
  static const std::string INSTRUCTIONS =
      "While this scene is active, only the arrow keys, Return, and Escape can "
      "be used to navigate.";

  static constexpr std::array<int, scene::Controls::CONTROL_INPUT_MAX>
      CONTROL_INPUTS = {
          SDLK_LEFT, SDLK_RIGHT, SDLK_UP, SDLK_DOWN, SDLK_RETURN, SDLK_ESCAPE,
  };
} // namespace

scene::Controls::Controls()
    : Scene("controls"), input(0), font(nullptr), waitKey(false),
      changePlayer(0), changeKeyEnum(0) {}

scene::Controls::~Controls() {}

void scene::Controls::init() {
  Scene::init();
  {
    std::vector<std::unique_ptr<menu::IMenuElement>> elements;
    const auto &key_config = input::getrKeyConfig();
    for (int i = 0; i < 2; i++) {
      for (int j = 0; j < input::KEY_INPUT_MAX; j++) {
        auto textButton = std::make_unique<menu::TextButtonA>();
        if (font) {
          textButton->setFont(font);
          textButton->setPos(sys::WINDOW_WIDTH / 2 + i * sys::WINDOW_WIDTH / 4,
                             64 + (j + 1) * font->getcImage()->getH());
          textButton->setColorActive(255, 255, 255);
          textButton->setColorInctive(127, 127, 127);

          textButton->setText(
              SDL_GetKeyName(key_config[i * input::KEY_INPUT_MAX + j]));
        }
        textButton->setSelectSound(sndSelect);
        textButton->setAction([this, i, j]() {
          waitKey = true;
          changePlayer = i;
          changeKeyEnum = j;
        });
        elements.emplace_back(std::move(textButton));
      }
    }
    submenu.setElements(std::move(elements));
    submenu.setInputMask(INPUT_DOWN, INPUT_UP);
    submenu.setMenuSound(sndMenu);
    submenu.setBackSound(sndBack);
    submenu.setAction([this]() {
      writeControls();
      setScene(SCENE_TITLE);
    });
  }
}

void scene::Controls::applyInput(int input) {
  if (!waitKey) {
    for (int i = 0; i < CONTROL_INPUT_MAX; i++) {
      if (CONTROL_INPUTS[i] == input) {
        this->input |= 1 << i;
        break;
      }
    }
  }
  else {
    auto &key_config = input::getrKeyConfig();
    switch (input) {
    case SDLK_ESCAPE:
    case SDLK_F1:
    case SDLK_F2:
    case SDLK_F3:
    case SDLK_F4:
    case SDLK_F5:
    case SDLK_F6:
    case SDLK_F7:
    case SDLK_F8:
    case SDLK_F9:
    case SDLK_F10:
    case SDLK_F11:
    case SDLK_F12:
      sndInvalid->play();
      break;
    default: {
      key_config[changePlayer * input::KEY_INPUT_MAX + changeKeyEnum] = input;
      sndMenu->play();
      break;
    }
    }
    waitKey = false;
    menu::TextButtonA *textButton =
        dynamic_cast<menu::TextButtonA *>(submenu.getElementAt(
            changePlayer * input::KEY_INPUT_MAX + changeKeyEnum));
    if (textButton) {
      textButton->setText(SDL_GetKeyName(
          key_config[changePlayer * input::KEY_INPUT_MAX + changeKeyEnum]));
    }
  }
}

void scene::Controls::think() {
  Scene::think();

  if (!waitKey) {
    submenu.think();

    if (input & INPUT_LEFT) {
      submenu.setIndex((submenu.getIndex() + input::KEY_INPUT_MAX) %
                       (2 * input::KEY_INPUT_MAX));
      input &= ~INPUT_LEFT;
      sndMenu->play();
    }
    if (input & INPUT_RIGHT) {
      submenu.setIndex((submenu.getIndex() + input::KEY_INPUT_MAX) %
                       (2 * input::KEY_INPUT_MAX));
      input &= ~INPUT_RIGHT;
      sndMenu->play();
    }

    submenu.doInput(input);
    input = 0;
  }
  else {
    menu::TextButtonA *textButton =
        dynamic_cast<menu::TextButtonA *>(submenu.getElementAt(
            changePlayer * input::KEY_INPUT_MAX + changeKeyEnum));
    if (textButton) {
      textButton->setText("...");
    }
  }
}

void scene::Controls::reset() {
  Scene::reset();
  submenu.reset();

  input = 0;
}

void scene::Controls::draw() const {
  Scene::draw();

  font->drawText(sys::WINDOW_WIDTH / 4, 64, "Controls");
  font->drawText(sys::WINDOW_WIDTH / 2, 64, "Player 1");
  font->drawText(sys::WINDOW_WIDTH * 3 / 4, 64, "Player 2");
  for (int i = 0; i < input::KEY_INPUT_MAX; i++) {
    font->drawText(sys::WINDOW_WIDTH / 4,
                   64 + (i + 1) * font->getcImage()->getH(),
                   input::KEY_LABELS[i]);
  }

  submenu.draw();

  font->setScale(0.5f);
  int width = font->getTextWidth(INSTRUCTIONS);
  font->drawText((sys::WINDOW_WIDTH - width) / 2,
                 sys::WINDOW_HEIGHT - font->getcImage()->getH(), INSTRUCTIONS);
}

void scene::Controls::parseJSON(const nlohmann::ordered_json &j_obj) {
  if (j_obj.contains("font")) {
    font = getResourceT<Font>(j_obj["font"]);
  }
  Scene::parseJSON(j_obj);
}

void scene::Controls::writeControls() {
  // Read controls from file
  std::ofstream ofs(app::getConfigPath() + input::KEY_CFG_FILE);

  for (const auto &key : input::getrKeyConfig()) {
    ofs << std::to_string(key) << '\n';
  }
}
