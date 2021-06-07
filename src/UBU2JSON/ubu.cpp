#include "ubu.h"

#include "chars.h"

#include <array>
#include <memory>
#include <iostream>
#include <fstream>

namespace ubu {
	static std::array<std::string, static_cast<size_t>(Type::UBU_TYPE_MAX)> typesStr;
	static std::unique_ptr<UBU> currentType = nullptr;
}

void ubu::init() {
    std::cout << "Initializing\n";

#define UBU_TYPE_ENTRY(ENUM, STR) \
    typesStr[static_cast<size_t>(Type::ENUM)] = STR;

	UBU_TYPE_ENTRY(CHARACTER, "character");
    UBU_TYPE_ENTRY(SPRITES, "sprites");
    UBU_TYPE_ENTRY(SOUNDS, "sounds");
    UBU_TYPE_ENTRY(VOICES, "voices");

#undef UBU_TYPE_ENTRY
}

bool ubu::load(const std::string& type, const std::string& input, const std::string& output) {
#define UBU_LOAD_TYPE(STR, CLASS) \
    if (type == STR) { \
        std::cout << "Loading: " STR "\n"; \
		currentType = std::make_unique<CLASS>(input); \
	}

	UBU_LOAD_TYPE("character", Character);
    UBU_LOAD_TYPE("sprites", Sprites);
    UBU_LOAD_TYPE("sounds", Sounds);
    UBU_LOAD_TYPE("voices", Voices);

#undef UBU_LOAD_TYPE

    std::cout << "Parsing\n";
    if (currentType->parse()) {
        std::cout << "Writing\n";
        currentType->write(output);
        return true;
    }

    std::cerr << "Failed\n";

    return false;
}

void ubu::usage() {
    std::cerr << R"(usage: UBU2JSON <ubu-type> <input-file> [<output-file>]

<ubu-type> must be one of the following:
)";
    for (const auto& type : typesStr) {
        std::cerr << "    " << type << '\n';
    }

    std::cerr << R"(
<input-file> must be a valid ubu file of that type, relative to the executable

[<output-file>] writes to a file, relative to where this was run
)";
}

ubu::UBU::UBU(const std::string& file) 
    : parser(file), j_obj()
{}

void ubu::UBU::write(const std::string& output) const {
    std::string dump = j_obj.dump(4);
    std::cout << dump << '\n';
    if (!output.empty()) {
        std::ofstream ofs(output);
        ofs << dump << '\n';
    }
}
