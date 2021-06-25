#ifndef PARSER_JSON_PARSER_H
#define PARSER_JSON_PARSER_H

#include <nlohmann/json.hpp>

namespace parser {
  template <typename T> T parseJSON(const nlohmann::ordered_json &j_obj);
  template <typename T> nlohmann::ordered_json parseObject(const T &obj);
} // namespace parser

#endif // PARSER_JSON_PARSER_H
