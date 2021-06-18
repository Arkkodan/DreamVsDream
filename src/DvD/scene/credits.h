#ifndef DVD_SCENE_CREDITS_H
#define DVD_SCENE_CREDITS_H

#include "scene_base.h"

#include <vector>

#include "../font.h"

namespace scene {

  /// @brief In-game credits
  class Credits : public Scene {
  public:
    Credits();
    ~Credits();

    void think() override final;
    void draw() const override final;
    void reset() override final;
    void parseLine(Parser &parser) override final;
    void parseJSON(const nlohmann::ordered_json &j_obj) override final;

  private:
    static constexpr auto CREDITS_OFFSET = 20;

  private:
    // Logic
    mutable bool done;
    float oy;
    int timer_start;
    int timer_scroll;
    float secret_alpha;

    // Data
    Image logo;
    Font *font;
    int c_lines;
    std::vector<std::string> lines;

    uint8_t title_r;
    uint8_t title_g;
    uint8_t title_b;

    uint8_t name_r;
    uint8_t name_g;
    uint8_t name_b;
  };
} // namespace scene

#endif // DVD_SCENE_CREDITS_H
