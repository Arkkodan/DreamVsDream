#include "chars.h"

#if (_MSC_VER) // MSVC uses _stricmp instead of strcasecmp
#define strcasecmp(str1, str2) _stricmp(str1, str2)
#endif

ubu::Character::Character(const std::string &file) : UBU(file) {}

bool ubu::Character::parse() {
  if (!parser.exists()) {
    return false;
  }

  while (parser.parseLine()) {
    if (parser.is("NAME", 1)) {
      j_obj["name"] = parser.getArg(1);
    }
    else if (parser.is("GROUP", 1)) {
      j_obj["group"] = parser.getArgInt(1);
    }
    else if (parser.is("DEFENSE", 1)) {
      j_obj["defense"] = parser.getArgFloat(1);
    }
    else if (parser.is("HEIGHT", 1)) {
      j_obj["height"] = parser.getArgInt(1);
    }
    else if (parser.is("WIDTH", 2)) {
      j_obj["width"] = {
          {"left", parser.getArgInt(1)},
          {"right", parser.getArgInt(2)},
      };
    }
    else if (parser.is("GRAVITY", 1)) {
      j_obj["gravity"] = parser.getArgFloat(1);
    }
    else if (parser.is("PALETTES", 1)) {
      j_obj["palettes"] = parser.getArgInt(1);
    }
  }

  return true;
}

ubu::Sprites::Sprites(const std::string &file) : UBU(file) {}

bool ubu::Sprites::parse() {
  if (!parser.exists()) {
    return false;
  }

  int spriteIndex = -1;
  std::string name = "";
  int hitBoxIndex = 0;
  int aHitBoxIndex = 0;
  int currentHitBoxCount = 0;
  int currentAHitBoxCount = 0;

  while (parser.parseLine()) {
    int argc = parser.getArgC();

    if (parser.isGroup()) {
      spriteIndex++;
      j_obj[spriteIndex]["image"] = parser.getArg(0);

      hitBoxIndex = 0;
      aHitBoxIndex = 0;
      currentHitBoxCount = 0;
      currentAHitBoxCount = 0;

      if (argc == 5) {
        j_obj[spriteIndex]["pos"]["x"] = parser.getArgInt(1);
        j_obj[spriteIndex]["pos"]["y"] = parser.getArgInt(2);

        currentHitBoxCount = parser.getArgInt(3);
        currentAHitBoxCount = parser.getArgInt(4);
      }
      else {
        j_obj[spriteIndex]["pos"]["x"] = 0;
        j_obj[spriteIndex]["pos"]["y"] = 0;
      }
    }
    else {
      if (argc == 4) {
        if (hitBoxIndex < currentHitBoxCount) {
          // j_obj[spriteIndex]["hitBoxes"][hitBoxIndex]["pos"]["x"] =
          // parser.getArgInt(0);
          // j_obj[spriteIndex]["hitBoxes"][hitBoxIndex]["pos"]["y"] =
          // parser.getArgInt(1);
          // j_obj[spriteIndex]["hitBoxes"][hitBoxIndex]["size"]["x"] =
          // parser.getArgInt(2);
          // j_obj[spriteIndex]["hitBoxes"][hitBoxIndex]["size"]["y"] =
          // parser.getArgInt(3);
          j_obj[spriteIndex]["hitBoxes"][hitBoxIndex]["x"] =
              parser.getArgInt(0);
          j_obj[spriteIndex]["hitBoxes"][hitBoxIndex]["y"] =
              parser.getArgInt(1);
          j_obj[spriteIndex]["hitBoxes"][hitBoxIndex]["w"] =
              parser.getArgInt(2);
          j_obj[spriteIndex]["hitBoxes"][hitBoxIndex]["h"] =
              parser.getArgInt(3);
          hitBoxIndex++;
        }
        else if (aHitBoxIndex < currentAHitBoxCount) {
          // j_obj[spriteIndex]["aHitBoxes"][aHitBoxIndex]["pos"]["x"] =
          // parser.getArgInt(0);
          // j_obj[spriteIndex]["aHitBoxes"][aHitBoxIndex]["pos"]["y"] =
          // parser.getArgInt(1);
          // j_obj[spriteIndex]["aHitBoxes"][aHitBoxIndex]["size"]["x"] =
          // parser.getArgInt(2);
          // j_obj[spriteIndex]["aHitBoxes"][aHitBoxIndex]["size"]["y"] =
          // parser.getArgInt(3);
          j_obj[spriteIndex]["aHitBoxes"][aHitBoxIndex]["x"] =
              parser.getArgInt(0);
          j_obj[spriteIndex]["aHitBoxes"][aHitBoxIndex]["y"] =
              parser.getArgInt(1);
          j_obj[spriteIndex]["aHitBoxes"][aHitBoxIndex]["w"] =
              parser.getArgInt(2);
          j_obj[spriteIndex]["aHitBoxes"][aHitBoxIndex]["h"] =
              parser.getArgInt(3);
          aHitBoxIndex++;
        }
      }
    }
  }

  return true;
}

ubu::Sounds::Sounds(const std::string &file) : UBU(file) {}

bool ubu::Sounds::parse() {
  if (!parser.exists()) {
    return false;
  }

  int soundslistIndex = -1;
  std::string name = "";

  while (parser.parseLine()) {
    if (parser.isGroup()) {
      soundslistIndex++;
      j_obj[soundslistIndex]["name"] = parser.getArg(0);
    }
    else {
      j_obj[soundslistIndex]["sounds"].emplace_back(parser.getArg(0));
    }
  }

  return true;
}

ubu::Voices::Voices(const std::string &file) : UBU(file) {}

bool ubu::Voices::parse() {
  if (!parser.exists()) {
    return false;
  }

  int voiceslistIndex = -1;
  std::string name = "";

  while (parser.parseLine()) {
    if (parser.isGroup()) {
      voiceslistIndex++;
      j_obj[voiceslistIndex]["name"] = parser.getArg(0);
      j_obj[voiceslistIndex]["percent"] = parser.getArgInt(1);
    }
    else {
      // j_obj[voiceslistIndex]["voices"].emplace_back(parser.getArg(0));
      j_obj[voiceslistIndex]["sounds"].emplace_back(parser.getArg(0));
    }
  }

  return true;
}

ubu::States::States(const std::string &file) : UBU(file) {}

bool ubu::States::parse() {
  if (!parser.exists()) {
    return false;
  }

  int stateIndex = -1;
  int stepIndex = 0;

  while (parser.parseLine()) {
    if (parser.isGroup()) {
      stateIndex++;
      j_obj[stateIndex]["name"] = parser.getArg(0);
      stepIndex = 0;
    }
    else {
      std::string command = parser.getArg(0);
      // j_obj[stateIndex]["steps"][stepIndex]["command"] = command;

#define IF(COMMAND)                             \
  if (!strcasecmp(command.c_str(), #COMMAND)) { \
    auto &j_cmd = j_obj[stateIndex]["steps"][stepIndex][#COMMAND];
#define ELIF(COMMAND) \
  }                   \
  else IF(COMMAND)
#define ENDIF }

      IF(Sprite)
      j_cmd["image"] = parser.getArg(1);
      j_cmd["frames"] = parser.getArgInt(2);
      ELIF(Move)
      j_cmd["x"] = parser.getArgFloat(1);
      j_cmd["y"] = parser.getArgFloat(2);
      ELIF(Scale)
      j_cmd = parser.getArgFloat(1);
      ELIF(Type)
      j_cmd["act"] = parser.getArg(1);
      j_cmd["move"] = parser.getArg(2);
      ELIF(Ctrl)
      j_cmd = parser.getArgBool(1, false);
      ELIF(Attack)
      j_cmd["damage"] = parser.getArgFloat(1);
      j_cmd["type"] = parser.getArg(2);
      j_cmd["knockback"]["x"] = parser.getArgFloat(3);
      j_cmd["knockback"]["y"] = parser.getArgFloat(4);
      j_cmd["sound"] = parser.getArg(5);
      j_cmd["cancelable"] = parser.getArgBool(6, false);
      ELIF(Sound)
      j_cmd = parser.getArg(1);
      ELIF(Say)
      j_cmd = parser.getArg(1);
      ELIF(Heal)
      j_cmd = parser.getArgFloat(1);
      ELIF(Super)
      j_cmd = parser.getArgFloat(1);
      ELIF(Special)
      j_cmd = parser.getArgBool(1, false);
      ELIF(Force)
      j_cmd["x"] = parser.getArgFloat(1);
      j_cmd["y"] = parser.getArgFloat(2);
      ELIF(Shoot)
      j_cmd["projectile"] = parser.getArg(1);
      j_cmd["vel"]["x"] = parser.getArgFloat(2);
      j_cmd["vel"]["y"] = parser.getArgFloat(3);
      ELIF(OnHit)
      j_cmd = parser.getArg(1);
      ELIF(Destroy)
      j_cmd = parser.getArgBool(1, true);
      ELIF(Invincible)
      j_cmd = parser.getArgBool(1, false);
      ELIF(Gravity)
      j_cmd = parser.getArgBool(1, true);
      ELIF(Airdash)
      j_cmd = parser.getArgBool(1, false);
      ELIF(DoubleJump)
      j_cmd = parser.getArgBool(1, false);
      ELIF(Mirror)
      j_cmd = parser.getArgBool(1, false);
      ELIF(Bounce)
      j_cmd["knockback"]["x"] = parser.getArgFloat(1);
      j_cmd["knockback"]["y"] = parser.getArgFloat(2);
      j_cmd["frames"] = parser.getArgInt(3);
      ELIF(HitSpark)
      j_cmd = parser.getArg(1);
      ELIF(Effect)
      j_cmd["name"] = parser.getArg(1);
      j_cmd["displace"]["x"] = parser.getArgInt(2);
      j_cmd["displace"]["y"] = parser.getArgInt(3);
      j_cmd["type"] = parser.getArg(4);
      j_cmd["mirror"] = parser.getArgBool(5, false);
      j_cmd["speed"] = parser.getArgInt(6);
      j_cmd["loops"] = parser.getArgInt(7);
      ELIF(Cancel)
      j_cmd = parser.getArg(1);
      ELIF(Knockdown)
      j_cmd = nullptr;
      ;
      ELIF(Alpha)
      j_cmd = parser.getArgFloat(1);
      ELIF(Stun)
      j_cmd = parser.getArgInt(1);
      ENDIF
#undef ENDIF
#undef ELIF
#undef IF
      stepIndex++;
    }
  }

  return true;
}

ubu::Commands::Commands(const std::string &file) : UBU(file) {}

bool ubu::Commands::parse() {
  if (!parser.exists()) {
    return false;
  }

  int commandIndex = -1;
  int targetIndex = -1;
  int conditionIndex = 0;

  while (parser.parseLine()) {
    int argc = parser.getArgC();

    if (parser.isGroup()) {
      if (argc < 1) {
        continue;
      }

      commandIndex++;
      targetIndex = -1;
      conditionIndex = 0;

      for (int i = 0; i < argc; i++) {
        j_obj[commandIndex]["inputs"].emplace_back(parser.getArg(i));
      }
    }
    else if (parser.getArg(0).front() == '@') {
      targetIndex++;
      j_obj[commandIndex]["targets"][targetIndex]["state"] =
          parser.getArg(0).substr(1);
      conditionIndex = 0;
    }
    else {
      j_obj[commandIndex]["targets"][targetIndex]["conditions"]
           [conditionIndex] = parser.getArg(0);
      conditionIndex++;
    }
  }

  return true;
}
