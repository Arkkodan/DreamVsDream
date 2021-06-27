#include "voices.h"

template <>
parser::VoiceGroupsStruct parser::parseJSON<parser::VoiceGroupsStruct>(
    const nlohmann::ordered_json &j_obj) {
  VoiceGroupsStruct vgs;
  if (!j_obj.is_array()) {
    throw std::runtime_error("Passed in JSON file is not a JSON array");
  }

  vgs.reserve(j_obj.size());
  for (const auto &j_vg : j_obj) {
    VoiceGroup vg;
    vg.name = j_vg.value("name", "null");
    vg.percent = j_vg.value("percent", 100);
    if (!j_vg.contains("sounds")) {
      throw std::runtime_error(
          "Passed in JSON file is not a JSON array containing sounds");
    }
    const auto &j_sounds = j_vg["sounds"];
    if (j_sounds.is_array()) {
      vg.sounds.reserve(j_vg.size());
      for (const auto &j_sound : j_sounds) {
        vg.sounds.emplace_back(j_sound);
      }
    }
    else {
      throw std::runtime_error("Passed in JSON file is not a JSON array "
                               "containing sounds as an array");
    }

    vgs.emplace_back(vg);
  }

  return vgs;
}

template <>
nlohmann::ordered_json
parser::parseObject<parser::VoiceGroupsStruct>(const VoiceGroupsStruct &obj) {
  nlohmann::ordered_json j_obj = nlohmann::ordered_json::array();
  for (size_t i = 0, size = obj.size(); i < size; i++) {
    j_obj[i]["name"] = obj[i].name;
    j_obj[i]["percent"] = obj[i].percent;
    j_obj[i]["sounds"] = nlohmann::ordered_json::array();
    for (const auto &sound : obj[i].sounds) {
      j_obj[i]["sounds"].emplace_back(sound);
    }
  }

  return j_obj;
}
