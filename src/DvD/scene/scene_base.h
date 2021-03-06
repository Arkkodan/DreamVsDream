#ifndef DVD_SCENE_SCENE_BASE_H
#define DVD_SCENE_SCENE_BASE_H

#include "../font.h"
#include "../image.h"
#include "../parser.h"
#include "../sound.h"

#include <nlohmann/json.hpp>

#include <cstdint>
#include <list>
#include <string>
#include <unordered_map>
#include <vector>

namespace scene {

  /// @brief Image for displaying, intended for scenes
  class SceneImage {
  public:
    SceneImage(Image &_image, float _x, float _y, float _parallax,
               Image::Render _render, float _xvel, float _yvel, bool wrap,
               int round);
    ~SceneImage();

    void think();
    void reset();
    void draw(bool _stage) const;

  private:
    Image image;
    float x, y;
    float parallax; // for stages
    float xOrig, yOrig;
    float xvel, yvel;
    Image::Render render;
    bool wrap;
    int round;
  };

  /// @brief Scene base class
  class Scene {
  public:
    Scene(std::string name_);
    virtual ~Scene();

    // Functions
    virtual void init();

    virtual void think();
    virtual void reset();
    virtual void draw() const;

    void parseFile(std::string szFileName);
    virtual void parseLine(Parser &parser);

    virtual void parseJSON(const nlohmann::ordered_json &j_obj);

    std::string getResource(std::string szFileName,
                            std::string extension) const;

    /// @brief Template version of getResource
    template <typename T> T *getResourceT(const std::string &resource);

  protected:
    // Members
    std::string name;
    std::list<SceneImage> images;

    bool initialized;

    audio::Music bgm;
    bool bgmPlaying;

    // Video
    // Video* video;

    // audio::Sounds
    audio::Sound *sndMenu;
    audio::Sound *sndSelect;
    audio::Sound *sndBack;
    audio::Sound *sndInvalid;

  private:
    std::unordered_map<std::string, std::string> ext2dir;
    std::vector<Font *> deleteFontVector;
    std::vector<audio::Sound *> deleteSoundVector;
  };
} // namespace scene

#endif // DVD_SCENE_SCENE_BASE_H
