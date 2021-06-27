#ifndef PARSER_CHARS_SOUNDS_H
#define PARSER_CHARS_SOUNDS_H

#include "../JSON_parser.h"

#include <string>
#include <vector>

namespace parser {
  struct SoundGroup {
    std::string name = "null";
    std::vector<std::string> sounds;
  };

  using SoundGroupsStruct = std::vector<SoundGroup>;

  template <>
  SoundGroupsStruct
  parseJSON<SoundGroupsStruct>(const nlohmann::ordered_json &j_obj);

  template <>
  nlohmann::ordered_json
  parseObject<SoundGroupsStruct>(const SoundGroupsStruct &obj);
} // namespace parser

#endif // PARSER_CHARS_SOUNDS_H
