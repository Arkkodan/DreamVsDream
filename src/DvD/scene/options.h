#ifndef DVD_SCENE_OPTIONS_H
#define DVD_SCENE_OPTIONS_H

#include "scene_base.h"

#include "../font.h"
#include "../menu/submenu.h"

#include <array>

namespace scene {

  /// @brief Scene for modifying options
  class Options : public Scene {
  public:
    static int getDifficulty();
    static void setDifficulty(int difficulty);
    static int getWins();
    static void setWins(int wins);
    static int getTime();
    static void setTime(int time);
    static int getSfxVolume();
    static void setSfxVolume(int sfxVolume);
    static int getMusVolume();
    static void setMusVolume(int musVolume);
    static int getVoiceVolume();
    static void setVoiceVolume(int voiceVolume);
    static bool isEpilepsy();
    static void setEpilepsy(bool epilepsy);

  public:
    Options();
    ~Options();

    void init() override final;

    // Functions
    void think() override final;
    void reset() override final;
    void draw() const override final;

    void parseLine(Parser &parser) override final;
    void parseJSON(const nlohmann::ordered_json &j_obj) override final;

  private:
    enum {
      OPTION_MAIN_GAME,
      OPTION_MAIN_AUDIO,
      OPTION_MAIN_VIDEO,
      OPTION_MAIN_CREDITS,
      OPTION_MAIN_INTRO,
      OPTION_MAIN_CONTROLS,

      OPTION_MAIN_MAX
    };
    enum {
      OPTION_GAME_DIFFICULTY,
      OPTION_GAME_WINS,
      OPTION_GAME_TIME,

      OPTION_GAME_MAX
    };
    enum {
      OPTION_AUDIO_SFX_VOLUME,
      OPTION_AUDIO_MUS_VOLUME,
      OPTION_AUDIO_VOICE_VOLUME,

      OPTION_AUDIO_MAX
    };
    enum {
      OPTION_VIDEO_EPILEPSY,

      OPTION_VIDEO_MAX
    };

    static const std::array<std::string, OPTION_MAIN_MAX> submenuMainStrings;
    static const std::array<std::string, OPTION_GAME_MAX> submenuGameStrings;
    static const std::array<std::string, OPTION_AUDIO_MAX> submenuAudioStrings;
    static const std::array<std::string, OPTION_VIDEO_MAX> submenuVideoStrings;

    static int optionDifficulty;
    static int optionWins;
    static int optionTime;
    static int optionSfxVolume;
    static int optionMusVolume;
    static int optionVoiceVolume;
    static bool optionEpilepsy;

  private:
    // Members
    Font *menuFont;

    menu::MainSubmenuB submenuMain;
    menu::Submenu submenuGame;
    menu::Submenu submenuAudio;
    menu::Submenu submenuVideo;

    int madoPos;
    int madoDir;
    int madoFrame;

    Image madoImg;
    audio::Sound *madoSfxStep;
    audio::Sound *madoSfxPinch;

    audio::Voice *dame;
    audio::Voice *muri;

    int madoWakeTimer;

    uint8_t iR, iG, iB, aR, aG, aB;
    int aXOffset;

    std::vector<std::string> themes;
    int nThemes;
  };
} // namespace scene

#endif // DVD_SCENE_OPTIONS_H
