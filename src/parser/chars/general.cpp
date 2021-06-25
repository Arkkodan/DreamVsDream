#include "general.h"

template <>
parser::GeneralStruct
parser::parseJSON<parser::GeneralStruct>(const nlohmann::ordered_json &j_obj) {
  GeneralStruct gs;
  if (!j_obj.is_object()) {
    throw std::runtime_error("Passed in JSON file is not a JSON object");
  }

  gs.displayName = j_obj.value("displayName", "null");
  std::string groupName = j_obj.value("group", "Other");
  for (size_t i = 0; i < GROUP_ENUM_STRINGS.size(); i++) {
    if (GROUP_ENUM_STRINGS[i] == groupName) {
      gs.group = static_cast<int>(i);
      break;
    }
  }
  gs.hp = j_obj.value("hp", 0);
  if (j_obj.contains("collisionBox")) {
    auto &collisionBox = j_obj["collisionBox"];
    gs.collisionBox.width = collisionBox.value("width", 0);
    gs.collisionBox.height = collisionBox.value("height", 0);
  }
  gs.gravity = j_obj.value("gravity", 0.0f);
  gs.paletteCount = j_obj.value("paletteCount", 0);

  return gs;
}

template <>
nlohmann::ordered_json
parser::parseObject<parser::GeneralStruct>(const GeneralStruct &obj) {
  nlohmann::ordered_json j_obj;
  j_obj["displayName"] = obj.displayName;
  j_obj["group"] = GROUP_ENUM_STRINGS[obj.group];
  j_obj["hp"] = obj.hp;
  j_obj["collisionBox"]["width"] = obj.collisionBox.width;
  j_obj["collisionBox"]["height"] = obj.collisionBox.height;
  j_obj["gravity"] = obj.gravity;
  j_obj["paletteCount"] = obj.paletteCount;

  return j_obj;
}
