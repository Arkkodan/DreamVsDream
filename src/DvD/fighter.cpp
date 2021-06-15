#include "fighter.h"

#ifdef GAME
#include "../util/fileIO.h"
#include "error.h"
#include "file.h"
#include "graphics.h"
#include "resource_manager.h"
#include "shader_renderer/fighter_renderer.h"
#endif // GAME

namespace game {

#ifdef GAME
  audio::Sound *sndTransformYn = nullptr;
  audio::Sound *sndTransform2kki = nullptr;
  audio::Sound *sndTransformFlow = nullptr;

  static std::vector<audio::Sound *> deleteSoundVector;

  void init() {
    if (!(sndTransformYn = resource_manager::getResource<audio::Sound>(
              "Transform_yn.wav"))) {
      sndTransformYn = new audio::Sound;
      sndTransformYn->createFromFile("effects/Transform_yn.wav");
      deleteSoundVector.push_back(sndTransformYn);
    }
    if (!(sndTransform2kki = resource_manager::getResource<audio::Sound>(
              "Transform_2kki.wav"))) {
      sndTransform2kki = new audio::Sound;
      sndTransform2kki->createFromFile("effects/Transform_2kki.wav");
      deleteSoundVector.push_back(sndTransform2kki);
    }
    if (!(sndTransformFlow = resource_manager::getResource<audio::Sound>(
              "Transform_flow.wav"))) {
      sndTransformFlow = new audio::Sound;
      sndTransformFlow->createFromFile("effects/Transform_flow.wav");
      deleteSoundVector.push_back(sndTransformFlow);
    }
  }

  void deinit() {
    for (const auto *item : deleteSoundVector) {
      delete item;
    }
  }

  // Load/create a fighter
  void Fighter::create(std::string name_) {
    name = std::move(name_);

    // Get that file opened
    File file;
    std::string path = util::getPath("chars/" + name + ".char");
    if (!file.open(File::FILE_READ_NORMAL, path)) {
      error::die("Could not load fighter \"" + path + "\"");
    }

    // Read header
    dname = file.readStr();
    group = file.readByte();
    defense = file.readFloat();
    height = file.readWord();
    widthLeft = file.readWord();
    widthRight = file.readWord();
    gravity = file.readFloat();
    nPalettes = file.readByte();

    // Read palettes
    if (graphics::shader_support) {
      palettes.resize(nPalettes * 2);
    }
    else {
      palettes.clear();
    }

    uint8_t palette_first[256 * 3];
    uint8_t palette[256 * 3];

    // Initialize first colors to 0
    for (int i = 0; i < 3; i++) {
      palette_first[i] = 0;
      palette[i] = 0;
    }

    for (int i = 0; i < nPalettes * 2; i++) {
      uint8_t *pal;
      if (i == 0) {
        pal = palette_first;
      }
      else {
        pal = palette;
      }

      /*char buff[255 * 3];
      file.read(buff, 255 * 3);

      //Copy into the palette
      for(int j = 0; j < 255; j++)
      {
          pal[4 * j + 4] = buff[3 * j + 0];
          pal[4 * j + 5] = buff[3 * j + 1];
          pal[4 * j + 6] = buff[3 * j + 2];
          pal[4 * j + 7] = 255;
      }*/
      file.read(pal + 3, 255 * 3);

      // Make data a palette
      if (graphics::shader_support) {
        palettes[i].bindData(256, 1, GL_RGB, GL_UNSIGNED_BYTE, pal);
      }
    }

    // Read sprites
    nSprites = file.readWord();
    sprites.resize(nSprites);
    for (int i = 0; i < nSprites; i++) {
      sprites[i].atlas = &atlas_sprites;
      sprites[i].atlas_sprite = file.readWord();
      sprites[i].x = file.readWord();
      sprites[i].y = file.readWord();

      sprites[i].hitBoxes.init(file.readByte());
      for (int j = 0; j < sprites[i].hitBoxes.size; j++) {
        sprites[i].hitBoxes.boxes[j].pos.x = file.readWord();
        sprites[i].hitBoxes.boxes[j].pos.y = file.readWord();
        sprites[i].hitBoxes.boxes[j].size.x = file.readWord();
        sprites[i].hitBoxes.boxes[j].size.y = file.readWord();
      }
      sprites[i].aHitBoxes.init(file.readByte());
      for (int j = 0; j < sprites[i].aHitBoxes.size; j++) {
        sprites[i].aHitBoxes.boxes[j].pos.x = file.readWord();
        sprites[i].aHitBoxes.boxes[j].pos.y = file.readWord();
        sprites[i].aHitBoxes.boxes[j].size.x = file.readWord();
        sprites[i].aHitBoxes.boxes[j].size.y = file.readWord();
      }
    }
    atlas_sprites.create(file,
                         graphics::shader_support ? nullptr : palette_first);

    // Read sounds
    nSounds = file.readWord();
    sounds.resize(nSounds);
    for (int i = 0; i < nSounds; i++) {
      sounds[i].init(file.readWord());
      for (int j = 0; j < sounds[i].size; j++) {
        sounds[i].sounds[j].createFromEmbed(file);
      }
    }

    // Read voices
    nVoices = file.readWord();
    voices.resize(nVoices);
    for (int i = 0; i < nVoices; i++) {
      voices[i].init(file.readWord());
      voices[i].pct = file.readByte();
      for (int j = 0; j < voices[i].size; j++) {
        voices[i].voices[j].createFromEmbed(file);
      }
    }

    // Read states (gah)
    nStates = file.readWord();
    states.resize(nStates);
    for (int state = 0; state < nStates; state++) {
      states[state].size = file.readWord();
      states[state].steps.resize(states[state].size);
      file.read(states[state].steps.data(), states[state].size);
    }

    // Read commands
    nCommands = file.readWord();
    commands.resize(nCommands);
    for (int i = 0; i < nCommands; i++) {
      commands[i].generic = file.readWord();
      commands[i].comboC = file.readWord();
      for (int j = 0; j < commands[i].comboC; j++) {
        commands[i].combo[j] = file.readWord();
      }
      commands[i].targetC = file.readWord();
      commands[i].targets.resize(commands[i].targetC);
      for (int j = 0; j < commands[i].targetC; j++) {
        commands[i].targets[j].state = file.readWord();
        commands[i].targets[j].conditionC = file.readWord();
        for (int k = 0; k < commands[i].targets[j].conditionC; k++) {
          commands[i].targets[j].conditions[k] = file.readWord();
        }
      }
    }

    for (int i = 0; i < STATE_MAX; i++) {
      statesStandard[i] = file.readWord();
    }

    // Portraits
    select.createFromEmbed(file, nullptr);
    portrait.createFromEmbed(file, nullptr);
    special.createFromEmbed(file, nullptr);
    ender.createFromEmbed(file, nullptr);
    portrait_ui.createFromEmbed(file, graphics::shader_support ? nullptr
                                                               : palette_first);
  }
#endif

  Fighter::Fighter()
      : nPalettes(0), palettes(), nSprites(0), sprites(), name(), dname(),
        group(0), defense(0), height(0), widthLeft(0), widthRight(0), gravity(0)
#ifndef SPRTOOL
        ,
        nSounds(0), sounds(), nVoices(0), voices(), nCommands(0), commands(),
        nStates(0), states(), statesStandard()
#endif
  {
  }

  Fighter::~Fighter() {
#ifdef GAME
    if (graphics::shader_support) {
      palettes.clear();
    }
#endif
  }

#ifdef GAME

  void Fighter::draw(int sprite, int x, int y, bool mirror, float scale,
                     unsigned int palette, float alpha, float r, float g,
                     float b, float pct) const {
    if (graphics::shader_support) {
      graphics::setPalette(palettes[palette], alpha, r, g, b, pct);
      sprites[sprite].draw(x, y, mirror, scale);
      renderer::ShaderProgram::unuse();
    }
    else {
      if (palette) {
        renderer::FighterRenderer::setColor(150 / 255.0f, 150 / 255.0f,
                                            150 / 255.0f);
        renderer::FighterRenderer::setAlpha(alpha);
      }
      else {
        renderer::FighterRenderer::setColor(1.0f, 1.0f, 1.0f);
        renderer::FighterRenderer::setAlpha(alpha);
      }
      sprites[sprite].draw(x, y, mirror, scale);
    }
  }

  void Fighter::drawShadow(int sprite, int x, bool mirror, float scale) const {
    if (graphics::shader_support) {
      graphics::setPalette(palettes.front(), 0.5f, 0.0f, 0.0f, 0.0f, 1.0f);
      sprites[sprite].drawShadow(x, mirror, scale);
      renderer::ShaderProgram::unuse();
    }
    else {
      renderer::FighterRenderer::setColor(0.0f, 0.0f, 0.0f);
      renderer::FighterRenderer::setAlpha(0.5f);
      sprites[sprite].drawShadow(x, mirror, scale);
    }
  }

#endif

#ifndef SPRTOOL
  SoundGroup::SoundGroup() { size = 0; }

  SoundGroup::~SoundGroup() {}

  void SoundGroup::init(int size_) {
    size = size_;
    if (size) {
      sounds.resize(size);
    }
  }

  VoiceGroup::VoiceGroup() : size(0), pct(100), voices() {}

  VoiceGroup::~VoiceGroup() {}

  void VoiceGroup::init(int _size) {
    size = _size;
    if (size) {
      voices.resize(size);
    }
  }
#endif

  Command::Command() : comboC(0), generic(0), targetC(0), targets(), combo() {}

  Command::~Command() {}

  CommandTarget::CommandTarget() : conditions() {
    state = 0;
    conditionC = 0;
  }

  State::State() : size(0), steps() {}

  State::~State() {}

  void setBool(bool &dst, char src) {
    if (src == SBOOL_UNDEFINED) {
      return;
    }
    if (src == SBOOL_TRUE) {
      dst = true;
    }
    else {
      dst = false;
    }
  }

  void setStateBool(char &dst, bool src) {
    if (src) {
      dst = SBOOL_TRUE;
    }
    else {
      dst = SBOOL_FALSE;
    }
  }
} // namespace game
