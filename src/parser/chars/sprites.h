#ifndef PARSER_CHARS_SPRITES_H
#define PARSER_CHARS_SPRITES_H

#include "../JSON_parser.h"

#include <vector>

namespace parser {
  using SpritesStruct = std::vector<std::string>;

  template <>
  SpritesStruct parseJSON<SpritesStruct>(const nlohmann::ordered_json &j_obj);

  template <>
  nlohmann::ordered_json parseObject<SpritesStruct>(const SpritesStruct &obj);
} // namespace parser

#endif // PARSER_CHARS_SPRITES_H
