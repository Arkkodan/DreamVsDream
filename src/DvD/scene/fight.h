#ifndef DVD_SCENE_FIGHT_H
#define DVD_SCENE_FIGHT_H

#include "scene_base.h"

#include "../font.h"
#include "../player.h"

#include <glm/vec2.hpp>

#include <array>

namespace scene {

  /// @brief Helper object for drawing meters, primarily for SceneFight
  struct SceneMeter {
    void draw(float pct, bool mirror, bool flip) const;

    Image img;
    glm::ivec2 pos;
  };

  /// @brief Fighting game scene
  class Fight : public Scene {
  public:
    static void pause(int frames);
    static void shake(int frames);

    static game::Player &getrPlayerAt(int index);
    static glm::ivec2 &getrCameraPos();
    static glm::ivec2 &getrIdealCameraPos();
    static int getFramePauseTimer();

  public:
    enum {
      GAMETYPE_VERSUS,
      GAMETYPE_TRAINING,
    };

  public:
    Fight();
    ~Fight();

    void init() override final;

    // Functions
    void think() override final;
    void draw() const override final;
    void reset() override final;

    void knockout(int player);

    void parseLine(Parser &parser) override final;
    void parseJSON(const nlohmann::ordered_json &j_obj) override final;

    int getTimerRoundIn() const;
    int getTimerRoundOut() const;
    int getTimerKO() const;
    int getKOPlayer() const;
    int getRound() const;
    int getGameType() const;
    void setGameType(int gametype);

  private:
    static game::Player madotsuki;
    static game::Player poniko;
    static glm::ivec2 cameraPos;
    static glm::ivec2 idealCameraPos;
    static glm::ivec2 cameraShake;

    static int framePauseTimer;
    static int frameShakeTimer;

  private:
    Image hud;
    Image hud_tag;
    Image portraits;
    Image portraits_tag;
    Image timer;
    Font *timer_font;
    Image shine;
    Image shine_tag;

    // Bars and stuff
    SceneMeter meterHp;
    SceneMeter meterSuper;
    SceneMeter meterTag;
    SceneMeter meterStun;
    SceneMeter meterGuard;
    SceneMeter meterDpm;

    Image staticImg;
    audio::Sound *staticSnd;
    audio::Sound *fadeinSnd;
    audio::Sound *fadeoutSnd;

    std::array<Image, 5> round_splash;
    Image round_hud[5];
    int x_round_hud;
    int y_round_hud;

    std::array<Image, 3> ko; // KO, timeout, draw

    Font *combo;
    Image comboLeft;
    Image comboRight;
    unsigned int comboLeftOff;
    unsigned int comboRightOff;
    int comboLeftLast;
    int comboRightLast;
    int comboLeftTimer;
    int comboRightTimer;

    glm::ivec2 portraitPos;

    Image win;
    Font *win_font;
    audio::Music win_bgm;

    Image orb_null;
    Image orb_win;
    Image orb_draw;
    glm::ivec2 orb_pos;

    // Timers for various effects
    mutable int timer_flash;
    int timer_round_in;
    int timer_round_out;
    int timer_ko; // timeout/draw too

    // Logic
    int round; // What round is it?
    std::array<int, 2>
        wins; // How many times each player has won (including draws)
    std::array<std::array<int, 3>, 2>
        win_types; // What type of win was it? Win or draw?
    int winner;    // Who won the game?

    int ko_player; // Which player was ko'd/lost? 2 = draw
    int ko_type;   // Was it a ko or was it a timeout?

    int game_timer; // this is the actual game timer

    int gametype;
  };
} // namespace scene

#endif // DVD_SCENE_FIGHT_H
