#include <algorithm>
#include <iostream>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <string>

#include "../DvD/error.h"
#include "../DvD/fighter.h"
#include "../DvD/file.h"
#include "../DvD/parser.h"
#include "../DvD/player.h"
#include "../DvD/sys.h"
#include "../util/fileIO.h"

#ifdef _WIN32
#ifndef WINVER
#define WINVER 0x0500
#endif
#include <shlwapi.h>
#include <windows.h>
#endif

#if (_MSC_VER) // MSVC uses _stricmp instead of strcasecmp
#define strcasecmp(str1, str2) _stricmp(str1, str2)
#endif

// ATLAS DATA
struct AtlasOffset {
  std::string name;
  int atlas;
  int x;
  int y;
  int w;
  int h;
  int xShift;
  int yShift;
};

class AtlasList {
public:
  AtlasList();
  ~AtlasList();

  bool create(std::string szFileName);

  int nSprites;
  AtlasOffset *sprites;
};

AtlasList::AtlasList() {}

AtlasList::~AtlasList() { delete[] sprites; }

bool AtlasList::create(std::string szFileName) {
  File file;
  if (!file.open(File::FILE_READ_NORMAL, szFileName)) {
    return false;
  }

  nSprites = file.readWord();
  sprites = new AtlasOffset[nSprites];
  for (int i = 0; i < nSprites; i++) {
    sprites[i].name = file.readStr();
    sprites[i].atlas = file.readByte();
    sprites[i].x = file.readWord();
    sprites[i].y = file.readWord();
    sprites[i].w = file.readWord();
    sprites[i].h = file.readWord();
    sprites[i].xShift = file.readWord();
    sprites[i].yShift = file.readWord();
  }
  return true;
}

// Embedding files
void embedFile(File &out, const std::string &file) {
  // Load the file into memory
  FILE *f = util::ufopen(util::getPath(file), "rb");
  if (!f) {
    out.writeDword(0);
    return;
  }
  fseek(f, 0, SEEK_END);
  size_t size = ftell(f);
  fseek(f, 0, SEEK_SET);
  if (size == 0) {
    fclose(f);
    out.writeDword(0);
    return;
  }
  uint8_t *data = (uint8_t *)malloc(size);
  fread(data, size, 1, f);
  fclose(f);

  // Write the size/data
  out.writeDword(size);
  out.write(data, size);

  // Free
  free(data);
}

// Writing to memory
#define BYTE(v) stepWriteByte(&_i_step, _b_step, v)
#define WORD(v) stepWriteWord(&_i_step, _b_step, v)
#define DWORD(v) stepWriteDword(&_i_step, _b_step, v)
#define FLOAT(v) stepWriteFloat(&_i_step, _b_step, v)
#define STRING(v) stepWriteString(&_i_step, _b_step, v)

void stepWriteByte(int *index, void *buffer, int8_t value) {
  *((int8_t *)((char *)buffer + *index)) = value;
  *index += 1;
}

void stepWriteWord(int *index, void *buffer, int16_t value) {
  *((int16_t *)((char *)buffer + *index)) = value;
  *index += 2;
}

void stepWriteDword(int *index, void *buffer, int32_t value) {
  *((int32_t *)((char *)buffer + *index)) = value;
  *index += 4;
}

void stepWriteFloat(int *index, void *buffer, float value) {
  *((int32_t *)((char *)buffer + *index)) = value * sys::FLOAT_ACCURACY;
  *index += 4;
}

void stepWriteString(int *index, void *buffer, const std::string &value) {
  *((int8_t *)((char *)buffer + *index)) = value.size();
  memcpy((char *)buffer + *index + 1, value.c_str(), value.size());
  *index += value.size() + 1;
}

namespace game {
  const std::array<std::string, game::STATE_MAX> stateNames = {
      "Stand",      "Crouch",       "BeginStand", "BeginCrouch",

      "WalkF",      "WalkB",

      "CWalkF",     "CWalkB",

      "BDashF",     "DashF",        "EDashF",     "BDashB",
      "DashB",      "EDashB",

      "Jumping",    "BeginFall",    "Falling",    "Land",

      "JTech",

      "HitHigh",    "HitMid",       "HitLow",

      "Block",      "CBlock",       "JBlock",

      "KB",         "KBFall",       "KBFalling",  "KBBounce",
      "KBBouncing", "KBLand",       "OnBack",     "RecoverBack",

      "KP",         "KPFall",       "KPFalling",  "KPLand",
      "Prone",      "RecoverProne",

      "Victory",    "Defeat",
  };

  void Fighter::create(std::string name_) {
    name = name_;

    // General definition file
    Parser parser("chars/" + name + "/character.ubu");

    // Load stuff
    while (parser.parseLine()) {
      if (parser.is("NAME", 1)) {
        dname = parser.getArg(1);
      }
      else if (parser.is("GROUP", 1)) {
        group = parser.getArgFloat(1) - 1;
      }
      else if (parser.is("DEFENSE", 1)) {
        defense = parser.getArgFloat(1) * 0.01;
      }
      else if (parser.is("HEIGHT", 1)) {
        height = parser.getArgInt(1);
      }
      else if (parser.is("WIDTH", 2)) {
        widthLeft = parser.getArgInt(1);
        widthRight = parser.getArgInt(2);
      }
      else if (parser.is("GRAVITY", 1)) {
        gravity = parser.getArgFloat(1);
      }
      else if (parser.is("PALETTES", 1)) {
        nPalettes = parser.getArgInt(1);
      }
    }

    // Palettes
    palettes.resize(nPalettes * 255 * 3 * 2);

    for (int i = 0; i < nPalettes * 2; i++) {
      File act;
      std::string path;

      if (i < nPalettes) {
        path = util::getPath("chars/" + name + "/palettes/" +
                             util::toString(i + 1) + ".act");
        if (!act.open(File::FILE_READ_NORMAL, path)) {
          error::die("Cannot open \"" + path + "\"!");
        }
      }
      else {
        path = util::getPath("chars/" + name + "/palettes/" +
                             util::toString(i - nPalettes + 1) + "_nes.act");
        if (!act.open(File::FILE_READ_NORMAL, path)) {
          error::die("Cannot open \"!" + path + "\"!");
        }
      }
      act.seek(3);

      if (act.read(&palettes[i * 255 * 3], 255 * 3) != 1) {
        error::die("\"" + path + "\" not a valid ACT file!");
      }
    }

    // Sprites
    parser.open("chars/" + name + "/sprites.ubu");

    // First pass
    nSprites = 0;
    while (parser.parseLine()) {
      if (parser.isGroup()) {
        nSprites++;
      }
    }
    sprites.resize(nSprites);

    // Parse the damned sprites for god's sake
    // Second pass
    parser.reset();

    int hitboxCounter = 0;
    int attackCounter = 0;
    int i = -1;
    while (parser.parseLine()) {
      int argc = parser.getArgC();
      if (parser.isGroup()) {
        // There's a new sprite definition! Load that shit
        i++;
        hitboxCounter = 0;
        attackCounter = 0;

        sprites[i].name = parser.getArg(0);
        if (parser.getArgC() == 5) {
          sprites[i].x = parser.getArgInt(1);
          sprites[i].y = parser.getArgInt(2);

          sprites[i].hitBoxes.init(parser.getArgInt(3));
          sprites[i].aHitBoxes.init(parser.getArgInt(4));
        }
        else {
          sprites[i].x = 0;
          sprites[i].y = 0;

          sprites[i].hitBoxes.init(0);
          sprites[i].aHitBoxes.init(0);
        }
        continue;
      }

      if (argc == 4) {
        if (hitboxCounter < sprites[i].hitBoxes.size) {
          sprites[i].hitBoxes.boxes[hitboxCounter].pos.x = parser.getArgInt(0);
          sprites[i].hitBoxes.boxes[hitboxCounter].pos.y = parser.getArgInt(1);
          sprites[i].hitBoxes.boxes[hitboxCounter].size.x = parser.getArgInt(2);
          sprites[i].hitBoxes.boxes[hitboxCounter].size.y = parser.getArgInt(3);
          hitboxCounter++;
        }
        else if (attackCounter < sprites[i].aHitBoxes.size) {
          sprites[i].aHitBoxes.boxes[attackCounter].pos.x = parser.getArgInt(0);
          sprites[i].aHitBoxes.boxes[attackCounter].pos.y = parser.getArgInt(1);
          sprites[i].aHitBoxes.boxes[attackCounter].size.x =
              parser.getArgInt(2);
          sprites[i].aHitBoxes.boxes[attackCounter].size.y =
              parser.getArgInt(3);
          attackCounter++;
        }
      }
    }

    // Sounds
    parser.open("chars/" + name + "/sounds.ubu");

    // First pass (count sound groups)
    nSounds = 0;
    while (parser.parseLine()) {
      if (parser.isGroup()) {
        nSounds++;
      }
    }
    sounds.resize(nSounds);

    // Second pass (count sounds within groups, set group ids)
    parser.reset();
    int groupCounter = -1;
    int soundCounter = 0;
    while (parser.parseLine()) {
      if (parser.isGroup()) {
        if (groupCounter >= 0) {
          sounds[groupCounter].init(soundCounter);
        }
        soundCounter = 0;
        sounds[++groupCounter].name = parser.getArg(0);
      }
      else {
        soundCounter++;
      }
    }
    if (groupCounter >= 0 && soundCounter > 0) {
      sounds[groupCounter].init(soundCounter);
    }

    // Third pass (load sounds)
    parser.reset();
    groupCounter = -1;
    soundCounter = 0;
    while (parser.parseLine()) {
      if (parser.isGroup()) {
        groupCounter++;
        soundCounter = 0;
        continue;
      }

      sounds[groupCounter].sounds[soundCounter++] = parser.getArg(0);
    }

    // Voices
    parser.open("chars/" + name + "/voices.ubu");

    // First pass (count voice groups)
    nVoices = 0;
    while (parser.parseLine()) {
      if (parser.isGroup()) {
        nVoices++;
      }
    }
    voices.resize(nVoices);

    // Second pass (count voices within groups, set group ids)
    parser.reset();
    groupCounter = -1;
    int voiceCounter = 0;
    while (parser.parseLine()) {
      if (parser.isGroup()) {
        if (groupCounter >= 0) {
          voices[groupCounter].init(voiceCounter);
        }
        voiceCounter = 0;
        voices[++groupCounter].name = parser.getArg(0);
        voices[groupCounter].pct = parser.getArgInt(1);
      }
      else {
        voiceCounter++;
      }
    }
    if (groupCounter >= 0 && voiceCounter > 0) {
      voices[groupCounter].init(voiceCounter);
    }

    // Third pass (load voices)
    parser.reset();
    groupCounter = -1;
    voiceCounter = 0;
    while (parser.parseLine()) {
      if (parser.isGroup()) {
        groupCounter++;
        voiceCounter = 0;
        continue;
      }

      voices[groupCounter].voices[voiceCounter++] = parser.getArg(0);
    }

    // States (oh boy)
    parser.open("chars/" + name + "/states.ubu");

    // First pass (count states)
    nStates = 0;
    while (parser.parseLine()) {
      if (parser.isGroup()) {
        nStates++;
      }
    }
    states.resize(nStates);

    // Second pass (get state names)
    parser.reset();
    int stateCounter = 0;
    while (parser.parseLine()) {
      if (parser.isGroup()) {
        states[stateCounter++].name = parser.getArg(0);
      }
    }

    int _i_step = 0;
    unsigned char _b_step[2048];

#define _STEP(type)                      \
  if (!strcasecmp(key.c_str(), #type)) { \
    BYTE(STEP_##type);
#define STEP(type)                            \
  }                                           \
  else if (!strcasecmp(key.c_str(), #type)) { \
    BYTE(STEP_##type);
#define STEP_END() }

    // Third pass (load data)
    parser.reset();
    stateCounter = -1;
    while (parser.parseLine()) {
      if (parser.isGroup()) {
        // Increase the state counter, reset the step counter
        if (_i_step > 0) {
          states[stateCounter].size = _i_step;
          states[stateCounter].steps.resize(_i_step);
          memcpy(states[stateCounter].steps.data(), _b_step, _i_step);
        }
        stateCounter++;
        _i_step = 0;
      }
      else {
        std::string key = parser.getArg(0);

        _STEP(Sprite)
        int _i_sprite = 0;
        std::string sprite = parser.getArg(1);
        for (int i = 0; i < nSprites; i++) {
          if (!sprites[i].name.compare(sprite)) {
            _i_sprite = i;
            break;
          }
        }

        WORD(_i_sprite);
        WORD(parser.getArgInt(2));
        STEP(Move)
        FLOAT(parser.getArgFloat(1));
        FLOAT(parser.getArgFloat(2));
        STEP(Scale)
        FLOAT(parser.getArgFloat(1));
        STEP(Type)
        BYTE(parser.getArg(1).front());
        BYTE(parser.getArg(2).front());
        STEP(Ctrl)
        BYTE(parser.getArgBool(1, false));
        STEP(Attack)
        FLOAT(parser.getArgFloat(1) * 0.01);

        unsigned char type = 0;
        std::string ht = parser.getArg(2);
        if (!ht.compare("low")) {
          type = HT_LOW;
        }
        else if (!ht.compare("mid")) {
          type = HT_MID;
        }
        else if (!ht.compare("high")) {
          type = HT_HIGH;
        }
        BYTE(type);

        FLOAT(parser.getArgFloat(3));
        FLOAT(parser.getArgFloat(4));

        int sound = 0;
        std::string g = parser.getArg(5);
        for (int i = 0; i < nStates; i++) {
          if (!sounds[i].name.compare(g)) {
            sound = i;
            break;
          }
        }
        WORD(sound);

        BYTE(parser.getArgBool(6, false));
        STEP(Sound)
        std::string g = parser.getArg(1);
        int sound = -1;
        for (int i = 0; i < nSounds; i++) {
          if (!sounds[i].name.compare(g)) {
            sound = i;
            break;
          }
        }
        WORD(sound);
        STEP(Say)
        std::string g = parser.getArg(1);
        int voice = -1;
        for (int i = 0; i < nVoices; i++) {
          if (!voices[i].name.compare(g)) {
            voice = i;
            break;
          }
        }
        WORD(voice);
        STEP(Heal)
        FLOAT(parser.getArgFloat(1) * 0.01);
        STEP(Super)
        FLOAT(parser.getArgFloat(1) * 0.01);
        STEP(Special)
        BYTE(parser.getArgBool(1, false));
        STEP(Force)
        FLOAT(parser.getArgFloat(1));
        FLOAT(parser.getArgFloat(2));
        STEP(Shoot)
        int state = 0;
        std::string target = parser.getArg(1);
        for (int i = 0; i < nStates; i++) {
          if (!states[i].name.compare(target)) {
            state = i;
            break;
          }
        }
        WORD(state);
        FLOAT(parser.getArgFloat(2));
        FLOAT(parser.getArgFloat(3));
        STEP(OnHit)
        int state = 0;
        std::string target = parser.getArg(1);
        for (int i = 0; i < nStates; i++) {
          if (!states[i].name.compare(target)) {
            state = i;
            break;
          }
        }
        WORD(state);
        STEP(Destroy)
        STEP(Visible)
        BYTE(parser.getArgBool(1, true));
        STEP(Invincible)
        BYTE(parser.getArgBool(1, false));
        STEP(Gravity)
        BYTE(parser.getArgBool(1, true));
        STEP(Airdash)
        BYTE(parser.getArgBool(1, false));
        STEP(DoubleJump)
        BYTE(parser.getArgBool(1, false));
        STEP(Mirror)
        BYTE(parser.getArgBool(1, false));
        STEP(Bounce)
        FLOAT(parser.getArgFloat(1));
        FLOAT(parser.getArgFloat(2));
        BYTE(parser.getArgInt(3));
        STEP(HitSpark)
        STRING(parser.getArg(1));
        STEP(Effect)
        STRING(parser.getArg(1));
        WORD(parser.getArgInt(2));
        WORD(parser.getArgInt(3));
        std::string type = parser.getArg(4);
        if (!type.compare("screen")) {
          BYTE(0);
        }
        else if (!type.compare("stay")) {
          BYTE(1);
        }
        else if (!type.compare("follow")) {
          BYTE(2);
        }
        else {
          error::die(std::string("Unknown effect type: ") + type);
        }
        BYTE(parser.getArgBool(5, false));
        BYTE(parser.getArgInt(6));
        BYTE(parser.getArgInt(7));
        STEP(Cancel)
        int state = 0;
        std::string target = parser.getArg(1);
        for (int i = 0; i < nStates; i++) {
          if (!states[i].name.compare(target)) {
            state = i;
            break;
          }
        }
        WORD(state);
        STEP(Knockdown)
        STEP(Alpha)
        FLOAT(parser.getArgFloat(1));
        STEP(Stun)
        WORD(parser.getArgInt(1));
        STEP_END();
      }
    }

    // Second pass (count step sizes)
    if (stateCounter >= 0 && _i_step > 0) {
      states[stateCounter].size = _i_step;
      states[stateCounter].steps.resize(_i_step);
      memcpy(states[stateCounter].steps.data(), _b_step, _i_step);
    }

    // Commands
    parser.open("chars/" + name + "/commands.ubu");

    // First pass (count commands)
    nCommands = 0;
    while (parser.parseLine()) {
      if (parser.isGroup()) {
        if (parser.getArgC() < 1) {
          continue;
        }
        nCommands++;
      }
    }
    commands.resize(nCommands);

    // Second pass (process commands and count targets/conditions)
    parser.reset();
    int commandCounter = -1;
    while (parser.parseLine()) {
      int argc = parser.getArgC();
      if (parser.isGroup()) {
        if (argc < 1) {
          continue;
        }

        // Create the old buffer
        if (commandCounter >= 0) {
          commands[commandCounter].targets.resize(
              commands[commandCounter].targetC);
        }

        commandCounter++;

        // Load the command
        for (int k = 0; k < argc; k++) {
          std::string cmd = parser.getArg(k);

          int key = 0;
          bool generic = false;
          int shift = 0;
          for (int i = 0; cmd[i]; i++) {
            switch (cmd[i]) {
            case '~':
              shift = INPUT_RELSHIFT;
              break;
            case '1':
              key = (INPUT_LEFT | INPUT_DOWN) << shift;
              break;
            case '2':
              key = INPUT_DOWN << shift;
              break;
            case '3':
              key = (INPUT_RIGHT | INPUT_DOWN) << shift;
              break;
            case '4':
              key = INPUT_LEFT << shift;
              break;
            case '6':
              key = INPUT_RIGHT << shift;
              break;
            case '7':
              key = (INPUT_LEFT | INPUT_UP) << shift;
              break;
            case '8':
              key = INPUT_UP << shift;
              break;
            case '9':
              key = (INPUT_RIGHT | INPUT_UP) << shift;
              break;
            case 'd':
              key = INPUT_DOWN << shift;
              generic = true;
              break;
            case 'u':
              key = INPUT_UP << shift;
              generic = true;
              break;
            case 'b':
              key = INPUT_LEFT << shift;
              generic = true;
              break;
            case 'f':
              key = INPUT_RIGHT << shift;
              generic = true;
              break;
            case 'A':
              key |= INPUT_A << shift;
              break;
            case 'B':
              key |= INPUT_B << shift;
              break;
            case 'C':
              key |= INPUT_C << shift;
              break;
            }
          }

          commands[commandCounter].combo[k] = key;
          if (generic) {
            commands[commandCounter].generic |= (1 << k);
          }
          commands[commandCounter].comboC++;
        }

        continue;
      }

      // Count the targets
      if (parser.getArg(0).front() == '@') {
        // Its a new target, count it.
        commands[commandCounter].targetC++;
      }
    }
    if (commandCounter >= 0) {
      commands[commandCounter].targets.resize(commands[commandCounter].targetC);
    }

    // Third pass (load the targets/conditions)
    parser.reset();
    commandCounter = -1;
    int targetCounter = -1;
    while (parser.parseLine()) {
      // int argc = parser.getArgC();

      if (parser.isGroup()) {
        commandCounter++;
        targetCounter = -1;
        continue;
      }

      // What is this?
      if (parser.getArg(0).front() == '@') {
        // Its a new target!
        targetCounter++;
        std::string target = parser.getArg(0).substr(1);
        for (int i = 0; i < nStates; i++) {
          if (!states[i].name.compare(target)) {
            commands[commandCounter].targets[targetCounter].state = i;
            break;
          }
        }
      }
      else {
        // Behold, a condition
        std::string cndstr = parser.getArg(0);
        unsigned char &cnd =
            commands[commandCounter].targets[targetCounter].conditions
                [commands[commandCounter].targets[targetCounter].conditionC];
        if (cndstr.front() == '!') {
          cnd = CND_NOT;
          cndstr.erase(0, 1);
        }
        else {
          cnd = 0;
        }

        // Find out what this thing is
        if (!cndstr.compare("onground")) {
          cnd |= CND_ON_GROUND;
        }
        else if (!cndstr.compare("crouching")) {
          cnd |= CND_CROUCHING;
        }
        else if (!cndstr.compare("airdash")) {
          cnd |= CND_AIRDASH;
        }
        else if (!cndstr.compare("doublejump")) {
          cnd |= CND_DOUBLEJUMP;
        }

        commands[commandCounter].targets[targetCounter].conditionC++;
      }
    }

    // Load up all the standard state id's
    for (int i = 0; i < STATE_MAX; i++) {
      // Load the state
      int j = 0;
      bool found = false;
      for (; j < nStates; j++) {
        if (!states[j].name.compare(stateNames[i])) {
          found = true;
          break;
        }
      }

      if (found) {
        statesStandard[i] = j;
      }
      else {
        statesStandard[i] = -1;
      }
    }
  }
} // namespace game

int main(int argc, char **argv) {
#ifdef _WIN32
  // Get argc/argv
  int argcW = 0;
  WCHAR **argvW = CommandLineToArgvW(GetCommandLineW(), &argcW);
  if (argcW < 2) {
    LocalFree(argvW);
    return 1;
  }
  char *name8 = util::utf16to8(argvW[1]);
  LocalFree(argvW);
  std::string name = name8;
  free(name8);
#else
  std::string name = argv[1];
#endif
  if (argc < 2) {
    std::cerr << "error: no fighter specified." << std::endl;
    return 1;
  }

  game::Fighter fighter;
  fighter.create(name);

  std::string fighterPrefix = "chars/" + name;

  // Load also the atlas
  AtlasList atlas_list;
  std::string path = util::getPath(fighterPrefix + "/atlas/atlas.list");
  if (!atlas_list.create(path)) {
    error::die("Could not load atlas list \"" + path + "\".");
  }

  // Open the file
  File file;
  path = util::getPath(fighterPrefix + ".char");
  file.open(File::FILE_WRITE_NORMAL, path);

  // Write Header
  std::cout << "Writing header..." << std::endl;
  file.writeStr(fighter.dname);
  file.writeByte(fighter.group);
  file.writeFloat(fighter.defense);
  file.writeWord(fighter.height);
  file.writeWord(fighter.widthLeft);
  file.writeWord(fighter.widthRight);
  file.writeFloat(fighter.gravity);
  file.writeByte(fighter.nPalettes);

  // Write palettes
  std::cout << "Writing palettes..." << std::endl;
  file.write(fighter.palettes.data(), fighter.nPalettes * 255 * 3 * 2);

  // Write Sprites
  std::cout << "Writing sprite information..." << std::endl;
  file.writeWord(fighter.nSprites);
  for (int i = 0; i < fighter.nSprites; i++) {
    int sprite_num = 0;
    int ox = 0;
    int oy = 0;
    for (int j = 0; j < atlas_list.nSprites; j++) {
      if (!atlas_list.sprites[j].name.compare(fighter.sprites[i].name)) {
        ox = atlas_list.sprites[j].xShift;
        oy = atlas_list.sprites[j].yShift;
        sprite_num = j;
        break;
      }
    }

    file.writeWord(sprite_num);
    file.writeWord(fighter.sprites[i].x - ox);
    file.writeWord(fighter.sprites[i].y - oy);
    /*file.writeWord(fighter.sprites[i].img.w);
    file.writeWord(fighter.sprites[i].img.h);
    file.write(fighter.sprites[i].img.data,
    fighter.sprites[i].img.w*fighter.sprites[i].img.h);*/
    file.writeByte(fighter.sprites[i].hitBoxes.size);
    for (int j = 0; j < fighter.sprites[i].hitBoxes.size; j++) {
      file.writeWord(fighter.sprites[i].hitBoxes.boxes[j].pos.x);
      file.writeWord(fighter.sprites[i].hitBoxes.boxes[j].pos.y);
      file.writeWord(fighter.sprites[i].hitBoxes.boxes[j].size.x);
      file.writeWord(fighter.sprites[i].hitBoxes.boxes[j].size.y);
    }
    file.writeByte(fighter.sprites[i].aHitBoxes.size);
    for (int j = 0; j < fighter.sprites[i].aHitBoxes.size; j++) {
      file.writeWord(fighter.sprites[i].aHitBoxes.boxes[j].pos.x);
      file.writeWord(fighter.sprites[i].aHitBoxes.boxes[j].pos.y);
      file.writeWord(fighter.sprites[i].aHitBoxes.boxes[j].size.x);
      file.writeWord(fighter.sprites[i].aHitBoxes.boxes[j].size.y);
    }
  }

  // Write atlases
  std::cout << "Writing atlases..." << std::endl;

  // First write the sprite info
  file.writeWord(atlas_list.nSprites);
  for (int i = 0; i < atlas_list.nSprites; i++) {
    file.writeByte(atlas_list.sprites[i].atlas);
    file.writeWord(atlas_list.sprites[i].x);
    file.writeWord(atlas_list.sprites[i].y);
    file.writeWord(atlas_list.sprites[i].w);
    file.writeWord(atlas_list.sprites[i].h);
    // file.writeWord(atlas_list.sprites[i].xShift);
    // file.writeWord(atlas_list.sprites[i].yShift);
  }

  int nAtlases = 0;
  std::vector<std::string> files =
      util::listDirectory(util::getPath(fighterPrefix + "/atlas"), true);

  // See if all of our images exist first, and count them.
  // Thumbs.db, .DS_Store, etc could screw the list up.
  for (std::vector<std::string>::size_type i = 0; i < files.size(); i++) {
    if (std::find(files.begin(), files.end(), util::toString(i) + ".png") !=
        files.end()) {
      nAtlases++;
    }
  }

  if (!nAtlases) {
    error::die("Could not find any atlas graphics in \"" + fighterPrefix +
               "/atlas\"");
  }

  // Write the atlases
  file.writeByte(nAtlases);
  for (int i = 0; i < nAtlases; i++) {
    embedFile(file, fighterPrefix + "/atlas/" + util::toString(i) + ".png");
  }

  // Write sounds
  std::cout << "Writing sounds..." << std::endl;
  file.writeWord(fighter.nSounds);
  for (int i = 0; i < fighter.nSounds; i++) {
    file.writeWord(fighter.sounds[i].size);
    for (int j = 0; j < fighter.sounds[i].size; j++) {
      // file.writeStr(fighter.sounds[i].sounds[j]);
      embedFile(file, fighterPrefix + "/sounds/" + fighter.sounds[i].sounds[j] +
                          ".wav");
    }
  }

  // Write voices
  std::cout << "Writing voices..." << std::endl;
  file.writeWord(fighter.nVoices);
  for (int i = 0; i < fighter.nVoices; i++) {
    file.writeWord(fighter.voices[i].size);
    file.writeByte(fighter.voices[i].pct);
    for (int j = 0; j < fighter.voices[i].size; j++) {
      // file.writeStr(fighter.voices[i].voices[j]);
      embedFile(file, fighterPrefix + "/voices/" + fighter.voices[i].voices[j] +
                          ".wav");
    }
  }

  // Write states (gah)
  std::cout << "Writing states..." << std::endl;
  file.writeWord(fighter.nStates);
  for (int state = 0; state < fighter.nStates; state++) {
    file.writeWord(fighter.states[state].size);
    file.write(fighter.states[state].steps.data(), fighter.states[state].size);
  }

  // Write commands
  std::cout << "Writing commands..." << std::endl;
  file.writeWord(fighter.nCommands);
  for (int i = 0; i < fighter.nCommands; i++) {
    file.writeWord(fighter.commands[i].generic);
    file.writeWord(fighter.commands[i].comboC);
    for (int j = 0; j < fighter.commands[i].comboC; j++) {
      file.writeWord(fighter.commands[i].combo[j]);
    }
    file.writeWord(fighter.commands[i].targetC);
    for (int j = 0; j < fighter.commands[i].targetC; j++) {
      file.writeWord(fighter.commands[i].targets[j].state);
      file.writeWord(fighter.commands[i].targets[j].conditionC);
      for (int k = 0; k < fighter.commands[i].targets[j].conditionC; k++) {
        file.writeWord(fighter.commands[i].targets[j].conditions[k]);
      }
    }
  }

  // Write standard states
  std::cout << "Writing standard state index..." << std::endl;
  for (int i = 0; i < game::STATE_MAX; i++) {
    file.writeWord(fighter.statesStandard[i]);
  }

  // Write portraits
  std::cout << "Writing portraits..." << std::endl;
  embedFile(file, fighterPrefix + "/portraits/select.png");
  embedFile(file, fighterPrefix + "/portraits/portrait.png");
  embedFile(file, fighterPrefix + "/portraits/special.png");
  embedFile(file, fighterPrefix + "/portraits/super.png");
  embedFile(file, fighterPrefix + "/portraits/ui.png");

  return 0;
}
