#include "ubu.h"

#include "chars.h"
#include "scenes.h"
#include "stages.h"

#include <array>
#include <fstream>
#include <iostream>
#include <memory>

namespace ubu {
  static std::array<std::string, static_cast<size_t>(UBUType::UBU_TYPE_MAX)>
      typesStr;
  static std::unique_ptr<UBU> currentType = nullptr;
} // namespace ubu

void ubu::init() {
  std::cout << "Initializing\n";

#define UBU_TYPE_ENTRY(ENUM, STR) \
  typesStr[static_cast<size_t>(UBUType::ENUM)] = STR;

  UBU_TYPE_ENTRY(CHARACTER, "character");
  UBU_TYPE_ENTRY(SPRITES, "sprites");
  UBU_TYPE_ENTRY(SOUNDS, "sounds");
  UBU_TYPE_ENTRY(VOICES, "voices");
  UBU_TYPE_ENTRY(STATES, "states");
  UBU_TYPE_ENTRY(COMMANDS, "commands");

  UBU_TYPE_ENTRY(SCENE, "scene");
  UBU_TYPE_ENTRY(INTRO, "intro");
  UBU_TYPE_ENTRY(TITLE, "title");
  UBU_TYPE_ENTRY(SELECT, "select");
  UBU_TYPE_ENTRY(OPTIONS, "options");
  UBU_TYPE_ENTRY(FIGHT, "fight");
  UBU_TYPE_ENTRY(NETPLAY, "netplay");
  UBU_TYPE_ENTRY(CREDITS, "credits");

  UBU_TYPE_ENTRY(STAGE, "stage");

#undef UBU_TYPE_ENTRY
}

bool ubu::load(const std::string &type, const std::string &input,
               const std::string &output) {
#define UBU_LOAD_TYPE(STR, CLASS)                 \
  if (type == STR) {                              \
    std::cout << "Loading: " STR "\n";            \
    currentType = std::make_unique<CLASS>(input); \
  }

  UBU_LOAD_TYPE("character", Character);
  UBU_LOAD_TYPE("sprites", Sprites);
  UBU_LOAD_TYPE("sounds", Sounds);
  UBU_LOAD_TYPE("voices", Voices);
  UBU_LOAD_TYPE("states", States);
  UBU_LOAD_TYPE("commands", Commands);

  UBU_LOAD_TYPE("scene", Scene);
  UBU_LOAD_TYPE("intro", Intro);
  UBU_LOAD_TYPE("title", Title);
  UBU_LOAD_TYPE("select", Select);
  UBU_LOAD_TYPE("options", Options);
  UBU_LOAD_TYPE("fight", Fight);
  UBU_LOAD_TYPE("netplay", Netplay);
  UBU_LOAD_TYPE("credits", Credits);

  UBU_LOAD_TYPE("stage", Stage);

#undef UBU_LOAD_TYPE
  if (currentType.get()) {
    std::cout << "Parsing\n";
    if (currentType->parse()) {
      std::cout << "Writing\n";
      currentType->write(output);
      return true;
    }
  }

  std::cerr << "Failed\n";

  return false;
}

void ubu::usage() {
  std::cerr << R"(usage: UBU2JSON <ubu-type> <input-file> [<output-file>]

<ubu-type> must be one of the following:
)";
  for (const auto &type : typesStr) {
    std::cerr << "    " << type << '\n';
  }

  std::cerr << R"(
<input-file> must be a valid ubu file of that type, relative to the executable

[<output-file>] writes to a file, relative to where this was run
)";
}

ubu::UBU::UBU(const std::string &file) : parser(file), j_obj() {}

void ubu::UBU::write(const std::string &output) const {
  std::string dump = j_obj.dump(4);
  std::cout << dump << '\n';
  if (!output.empty()) {
    std::ofstream ofs(output);
    ofs << dump << '\n';
  }
}
