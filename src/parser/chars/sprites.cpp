#include "sprites.h"

template <>
parser::SpritesStruct
parser::parseJSON<parser::SpritesStruct>(const nlohmann::ordered_json &j_obj) {
  SpritesStruct ss;
  if (!j_obj.is_array()) {
    throw std::runtime_error("Passed in JSON file is not a JSON array");
  }

  ss.reserve(j_obj.size());
  for (const auto &sprite : j_obj) {
    ss.emplace_back(sprite);
  }

  return ss;
}

template <>
nlohmann::ordered_json
parser::parseObject<parser::SpritesStruct>(const SpritesStruct &obj) {
  nlohmann::ordered_json j_obj = nlohmann::ordered_json::array();
  for (const auto &sprite : obj) {
    j_obj.emplace_back(sprite);
  }

  return j_obj;
}
