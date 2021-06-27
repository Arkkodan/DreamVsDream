#include "sounds.h"

template <>
parser::SoundGroupsStruct parser::parseJSON<parser::SoundGroupsStruct>(
    const nlohmann::ordered_json &j_obj) {
  SoundGroupsStruct sgs;
  if (!j_obj.is_array()) {
    throw std::runtime_error("Passed in JSON file is not a JSON array");
  }

  sgs.reserve(j_obj.size());
  for (const auto &j_sg : j_obj) {
    SoundGroup sg;
    sg.name = j_sg.value("name", "null");
    if (!j_sg.contains("sounds")) {
      throw std::runtime_error(
          "Passed in JSON file is not a JSON array containing sounds");
    }
    const auto &j_sounds = j_sg["sounds"];
    if (j_sounds.is_array()) {
      sg.sounds.reserve(j_sg.size());
      for (const auto &j_sound : j_sounds) {
        sg.sounds.emplace_back(j_sound);
      }
    }
    else {
      throw std::runtime_error("Passed in JSON file is not a JSON array "
                               "containing sounds as an array");
    }

    sgs.emplace_back(sg);
  }

  return sgs;
}

template <>
nlohmann::ordered_json
parser::parseObject<parser::SoundGroupsStruct>(const SoundGroupsStruct &obj) {
  nlohmann::ordered_json j_obj = nlohmann::ordered_json::array();
  for (size_t i = 0, size = obj.size(); i < size; i++) {
    j_obj[i]["name"] = obj[i].name;
    j_obj[i]["sounds"] = nlohmann::ordered_json::array();
    for (const auto &sound : obj[i].sounds) {
      j_obj[i]["sounds"].emplace_back(sound);
    }
  }

  return j_obj;
}
