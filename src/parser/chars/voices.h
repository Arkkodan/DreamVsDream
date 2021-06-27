#ifndef PARSER_CHARS_VOICES_H
#define PARSER_CHARS_VOICES_H

#include "../JSON_parser.h"

#include <string>
#include <vector>

namespace parser {
  struct VoiceGroup {
    std::string name = "null";
    int percent = 100;
    std::vector<std::string> sounds;
  };

  using VoiceGroupsStruct = std::vector<VoiceGroup>;

  template <>
  VoiceGroupsStruct
  parseJSON<VoiceGroupsStruct>(const nlohmann::ordered_json &j_obj);

  template <>
  nlohmann::ordered_json
  parseObject<VoiceGroupsStruct>(const VoiceGroupsStruct &obj);
} // namespace parser

#endif // PARSER_CHARS_VOICES_H
