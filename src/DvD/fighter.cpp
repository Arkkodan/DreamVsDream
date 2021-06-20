#include "fighter.h"

#ifdef GAME
#include "../util/fileIO.h"
#include "error.h"
#include "file.h"
#include "graphics.h"
#include "player.h"
#include "shader_renderer/fighter_renderer.h"
#endif // GAME

namespace game {

#ifdef GAME

  void init() { initTransformSounds(); }

  void deinit() { deinitTransformSounds(); }

  // Load/create a fighter
  void Fighter::create(std::string name_) {
    bool shader_support = graphics::hasShaderSupport();

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
    if (shader_support) {
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
      if (shader_support) {
        palettes[i].bindData(256, 1, GL_RGB, GL_UNSIGNED_BYTE, pal);
      }
    }

    // Read sprites
    nSprites = file.readWord();
    sprites.resize(nSprites);
    for (int i = 0; i < nSprites; i++) {
      sprites[i].setAtlas(&atlas_sprites);
      sprites[i].setAtlasSprite(file.readWord());
      sprites[i].setX(file.readWord());
      sprites[i].setY(file.readWord());

      sprite::HitBoxGroup &hurtBoxes = sprites[i].getrDHurtBoxes();
      hurtBoxes.init(file.readByte());
      for (int j = 0; j < hurtBoxes.size; j++) {
        hurtBoxes.boxes[j].pos.x = file.readWord();
        hurtBoxes.boxes[j].pos.y = file.readWord();
        hurtBoxes.boxes[j].size.x = file.readWord();
        hurtBoxes.boxes[j].size.y = file.readWord();
      }
      sprite::HitBoxGroup &hitBoxes = sprites[i].getrAHitBoxes();
      hitBoxes.init(file.readByte());
      for (int j = 0; j < hitBoxes.size; j++) {
        hitBoxes.boxes[j].pos.x = file.readWord();
        hitBoxes.boxes[j].pos.y = file.readWord();
        hitBoxes.boxes[j].size.x = file.readWord();
        hitBoxes.boxes[j].size.y = file.readWord();
      }
    }
    atlas_sprites.create(file, shader_support ? nullptr : palette_first);

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
    portrait_ui.createFromEmbed(file, shader_support ? nullptr : palette_first);
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
    if (graphics::hasShaderSupport()) {
      palettes.clear();
    }
#endif
  }

#ifdef GAME

  void Fighter::draw(int sprite, int x, int y, bool mirror, float scale,
                     unsigned int palette, float alpha, float r, float g,
                     float b, float pct) const {
    if (graphics::hasShaderSupport()) {
      graphics::setPalette(palettes[palette], alpha, r, g, b, pct);
      sprites[sprite].draw(x, y, mirror, scale);
      renderer::ShaderProgram::unuse();
    }
    else {
      if (palette) {
        renderer::FighterRenderer::setColor(
            {150 / 255.0f, 150 / 255.0f, 150 / 255.0f});
        renderer::FighterRenderer::setAlpha(alpha);
      }
      else {
        renderer::FighterRenderer::setColor({1.0f, 1.0f, 1.0f});
        renderer::FighterRenderer::setAlpha(alpha);
      }
      sprites[sprite].draw(x, y, mirror, scale);
    }
  }

  void Fighter::drawShadow(int sprite, int x, bool mirror, float scale) const {
    if (graphics::hasShaderSupport()) {
      graphics::setPalette(palettes.front(), 0.5f, 0.0f, 0.0f, 0.0f, 1.0f);
      sprites[sprite].drawShadow(x, mirror, scale);
      renderer::ShaderProgram::unuse();
    }
    else {
      renderer::FighterRenderer::setColor({0.0f, 0.0f, 0.0f});
      renderer::FighterRenderer::setAlpha(0.5f);
      sprites[sprite].drawShadow(x, mirror, scale);
    }
  }

#endif // GAME

  std::string Fighter::getDataName() const { return name; }
  std::string Fighter::getDisplayName() const { return dname; }
  int Fighter::getGroup() const { return group; }
  float Fighter::getDefense() const { return defense; }
  int Fighter::getHeight() const { return height; }
  int Fighter::getWidthLeft() const { return widthLeft; }
  int Fighter::getWidthRight() const { return widthRight; }
  float Fighter::getGravity() const { return gravity; }
  int Fighter::getPaletteCount() const { return nPalettes; }
#ifdef COMPILER
  const std::vector<uint8_t> &Fighter::getcrPalettes() const {
    return palettes;
  }
#else
  const std::vector<renderer::Texture2D> &Fighter::getcrPalettes() const {
    return palettes;
  }
#endif
  int Fighter::getSpriteCount() const { return nSprites; }
  const sprite::Sprite *Fighter::getcSpriteAt(int index) const {
    return &sprites[index];
  }
  sprite::Sprite *Fighter::getSpriteAt(int index) { return &sprites[index]; }
#ifndef SPRTOOL
  int Fighter::getSoundGroupCount() const { return nSounds; }
  const SoundGroup *Fighter::getcSoundGroupAt(int index) const {
    return &sounds[index];
  }
  int Fighter::getVoiceGroupCount() const { return nVoices; }
  const VoiceGroup *Fighter::getcVoiceGroupAt(int index) const {
    return &voices[index];
  }
  int Fighter::getCommandCount() const { return nCommands; }
  const Command *Fighter::getcCommandAt(int index) const {
    return &commands[index];
  }
  int Fighter::getStateCount() const { return nStates; }
  const State *Fighter::getcStateAt(int index) const { return &states[index]; }
  unsigned int Fighter::getStateStandardAt(int index) const {
    return statesStandard[index];
  }
#endif // !SPRTOOL
#ifdef GAME
  // Portraits
  const Image *Fighter::getcImageSelect() const { return &select; }
  const Image *Fighter::getcImagePortrait() const { return &portrait; }
  const Image *Fighter::getcImageSpecial() const { return &special; }
  const Image *Fighter::getcImageEnder() const { return &ender; }
  const Image *Fighter::getcImagePortraitUI() const { return &portrait_ui; }
#endif // GAME

#ifndef SPRTOOL
  void SoundGroup::init(int size_) {
    size = size_;
    if (size) {
      sounds.resize(size);
    }
  }

  void VoiceGroup::init(int _size) {
    size = _size;
    if (size) {
      voices.resize(size);
    }
  }
#endif

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
