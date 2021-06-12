#ifndef STAGE_H_INCLUDED
#define STAGE_H_INCLUDED

#include "animation.h"
#include "scene/scene_base.h"

#include <list>
#include <string>
#include <vector>

#define STAGE Stage::stages[Stage::stage]

/// @brief Stage class including creation, logic, and drawing
class Stage {
public:
  static int stage;
  static std::vector<Stage *> stages;

public:
  std::string name;
  std::list<scene::SceneImage> imagesAbove;
  std::list<scene::SceneImage> imagesBelow;
  audio::Music bgm;
  audio::Music bgm2;

  Animation thumbnail;

  bool exists;
  bool initialized;

  int width;
  int height;
  int widthAbs;
  int heightAbs;

  Stage();
  // Stage(const char* name);
  ~Stage();

  Stage(Stage &&other) = default;
  Stage &operator=(Stage &&other) = default;

  void create(std::string name);

  void init();

  void think();
  void reset();
  void draw(bool above) const;

  void bgmPlay();

  void parseFile(const std::string &szFileName);
  bool parseJSON(const std::string &name);
  std::string getResource(const std::string &szFileName,
                          const std::string &extension) const;

  bool isExists() const;

  static void ginit();
  static void deinit();
};

#endif // STAGE_H_INCLUDED
