#ifndef DVD_SCENE_VERSUS_H
#define DVD_SCENE_VERSUS_H

#include "scene_base.h"

namespace scene {

  /// @brief Versus splash cutscene
  class Versus : public Scene {
  public:
    Versus();
    ~Versus();

    void init() override final;

    // Members
    std::array<const Image *, 2> portraits;
    int timer1;
    int timer2;
    int timer3;
    int timer4;
    int timer5;
    int timer6;

    float timerF;

    // Functions
    void think() override final;
    void reset() override final;
    void draw() const override final;

    void parseLine(Parser &parser) override final;
  };
} // namespace scene

#endif // DVD_SCENE_VERSUS_H
