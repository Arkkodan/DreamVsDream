#include "stages.h"

ubu::Stage::Stage(const std::string &file)
    : UBU(file), imageAbove(0), imageBelow(0) {}

bool ubu::Stage::parse() {
  if (!parser.exists()) {
    return false;
  }

  while (parser.parseLine()) {
    if (parser.is("IMAGE_A", 2)) {
      parseImage(true);
      imageAbove++;
    }
    else if (parser.is("IMAGE_B", 2)) {
      parseImage(false);
      imageBelow++;
    }
    else if (parser.is("WIDTH", 1)) {
      // j_obj["width"] = parser.getArgInt(1);
      j_obj["entities"]["w_bounds"] = parser.getArgInt(1);
    }
    else if (parser.is("HEIGHT", 1)) {
      // j_obj["height"] = parser.getArgInt(1);
      j_obj["entities"]["h_ground"] = parser.getArgInt(1);
    }
    else if (parser.is("WIDTH_ABS", 1)) {
      // j_obj["widthAbs"] = parser.getArgInt(1);
      j_obj["camera"]["w"] = parser.getArgInt(1);
    }
    else if (parser.is("HEIGHT_ABS", 1)) {
      // j_obj["heightAbs"] = parser.getArgInt(1);
      j_obj["camera"]["h"] = parser.getArgInt(1);
    }
    else if (parser.is("BGM", 1)) {
      parseBGM(0);
    }
    else if (parser.is("BGM2", 1)) {
      parseBGM(1);
    }
  }

  return true;
}

void ubu::Stage::parseImage(bool above) {
  // std::string category = above ? "imageAbove" : "imageBelow";
  std::string category = above ? "imagesAhead" : "imagesBehind";
  int index = above ? imageAbove : imageBelow;

  int argc = parser.getArgC();

  j_obj[category][index]["image"] = parser.getArg(1);
  j_obj[category][index]["parallax"] = parser.getArgFloat(2);
  if (argc > 3) {
    j_obj[category][index]["round"] = parser.getArgInt(3);
    if (argc > 4) {
      // j_obj[category][index]["xvel"] = parser.getArgFloat(4);
      j_obj[category][index]["vel"]["x"] = parser.getArgFloat(4);
      if (argc > 5) {
        // j_obj[category][index]["yvel"] = parser.getArgFloat(5);
        j_obj[category][index]["vel"]["y"] = parser.getArgFloat(5);
        if (argc > 6) {
          j_obj[category][index]["wrap"] = parser.getArgBool(6, false);
        }
      }
    }
  }
}

void ubu::Stage::parseBGM(int index) {
  int argc = parser.getArgC();
  if (argc > 2) {
    j_obj["bgm"][index]["intro"] = parser.getArg(1);
    j_obj["bgm"][index]["loop"] = parser.getArg(2);
  }
  else {
    j_obj["bgm"][index]["loop"] = parser.getArg(1);
  }
}
