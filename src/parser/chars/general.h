#ifndef PARSER_CHARS_GENERAL_H
#define PARSER_CHARS_GENERAL_H

#include "../JSON_parser.h"

#include <array>

namespace parser {
  enum {
    YUME_NIKKI,
    YUME_2KKI,
    DOT_FLOW,
    OTHER,

    GROUP_MAX
  };

  const std::array<std::string, GROUP_MAX> GROUP_ENUM_STRINGS = {
      "Yume Nikki", "Yume 2kki", ".flow", "Other"};

  struct GeneralStruct {
    std::string displayName = "null";
    int group = OTHER;
    int hp = 0;
    struct {
      int width;
      int height;
    } collisionBox = {0, 0};
    float gravity = 0.0f;
    int paletteCount = 0;
  };

  template <>
  GeneralStruct parseJSON<GeneralStruct>(const nlohmann::ordered_json &j_obj);

  template <>
  nlohmann::ordered_json parseObject<GeneralStruct>(const GeneralStruct &obj);
} // namespace parser

#endif // PARSER_CHARS_GENERAL_H
