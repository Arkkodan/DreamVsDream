#ifndef DVD_STAGE_H
#define DVD_STAGE_H

#include "animation.h"
#include "scene/scene_base.h"

#include <list>
#include <string>
#include <vector>

#define STAGE Stage::getStage()

/// @brief Stage class including creation, logic, and drawing
class Stage {
private:
  static int stage;
  static std::vector<Stage *> stages;

public:
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

  Animation *getThumbnail();

  bool isExists() const;
  bool isInitialized() const;

  int getEntWidth() const;
  int getEntHeight() const;
  int getCamWidth() const;
  int getCamHeight() const;

public:
  static void ginit();
  static void deinit();
  static int getStageIndex();
  static void setStageIndex(int index);
  static Stage *getStage();
  static const std::vector<Stage *> &getcrStages();

private:
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
};

#endif // DVD_STAGE_H
