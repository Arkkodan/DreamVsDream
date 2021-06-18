#include "fight.h"

#include "options.h"
#include "scene.h"
#include "select.h"

#include "../../util/rng.h"
#include "../effect.h"
#include "../graphics.h"
#include "../resource_manager.h"
#include "../shader_renderer/fighter_renderer.h"
#include "../shader_renderer/texture2D_renderer.h"
#include "../stage.h"
#include "../sys.h"

#include <cstring>

game::Player scene::Fight::madotsuki;
game::Player scene::Fight::poniko;
util::Vector scene::Fight::cameraPos(0, 0);
util::Vector scene::Fight::idealCameraPos(0, 0);
util::Vector scene::Fight::cameraShake(0, 0);

int scene::Fight::framePauseTimer = 0;
int scene::Fight::frameShakeTimer = 0;

void scene::Fight::pause(int frames) { framePauseTimer += frames; }

void scene::Fight::shake(int frames) { frameShakeTimer += frames; }

game::Player &scene::Fight::getrPlayerAt(int index) {
  switch (index) {
  case 0:
    return madotsuki;
  case 1:
    return poniko;
  default:
    throw std::out_of_range("invalid Player index: " + std::to_string(index));
  }
}
util::Vector &scene::Fight::getrCameraPos() { return cameraPos; }
util::Vector &scene::Fight::getrIdealCameraPos() { return idealCameraPos; }
int scene::Fight::getFramePauseTimer() { return framePauseTimer; }

void scene::SceneMeter::draw(float pct, bool mirror, bool flip) const {
  if (pct > 0) {
    unsigned int imgW = img.getW();
    unsigned int imgH = img.getH();
    if (flip) {
      graphics::setRect(0, 0, imgW * pct, imgH);
    }
    else {
      graphics::setRect(imgW * (1 - pct), 0, imgW * pct + 1, imgH);
    }

    if (mirror) {
      if (flip) {
        img.draw<renderer::Texture2DRenderer>(
            sys::WINDOW_WIDTH - imgW - pos.x + imgW * (1 - pct), pos.y, true);
      }
      else {
        // Hack
        if (pct == 1.0f) {
          img.draw<renderer::Texture2DRenderer>(
              sys::WINDOW_WIDTH - imgW - pos.x, pos.y, true);
        }
        else {
          img.draw<renderer::Texture2DRenderer>(
              sys::WINDOW_WIDTH - imgW - pos.x + 1, pos.y, true);
        }
      }
    }
    else {
      if (flip) {
        img.draw<renderer::Texture2DRenderer>(pos.x, pos.y);
      }
      else {
        img.draw<renderer::Texture2DRenderer>(pos.x + imgW * (1 - pct), pos.y);
      }
    }
  }
}

scene::Fight::Fight() : Scene("fight") {
  gametype = GAMETYPE_TRAINING;
  staticSnd = fadeinSnd = fadeoutSnd = nullptr;
  timer_font = combo = win_font = nullptr;
  reset();
}

scene::Fight::~Fight() {}

void scene::Fight::init() { Scene::init(); }

void scene::Fight::parseLine(Parser &parser) {
  if (parser.is("HUD", 1)) {
    hud.createFromFile(getResource(parser.getArg(1), Parser::EXT_IMAGE));
  }
  else if (parser.is("HUD_TAG", 1)) {
    hud_tag.createFromFile(getResource(parser.getArg(1), Parser::EXT_IMAGE));
  }
  else if (parser.is("PORTRAITS", 1)) {
    portraits.createFromFile(getResource(parser.getArg(1), Parser::EXT_IMAGE));
  }
  else if (parser.is("PORTRAITS_TAG", 1)) {
    portraits_tag.createFromFile(
        getResource(parser.getArg(1), Parser::EXT_IMAGE));
  }
  else if (parser.is("TIMER", 2)) {
    timer.createFromFile(getResource(parser.getArg(1), Parser::EXT_IMAGE));
    timer_font = getResourceT<Font>(parser.getArg(2));
  }
  else if (parser.is("SHINE", 1)) {
    shine.createFromFile(getResource(parser.getArg(1), Parser::EXT_IMAGE));
  }
  else if (parser.is("SHINE_TAG", 1)) {
    shine_tag.createFromFile(getResource(parser.getArg(1), Parser::EXT_IMAGE));
  }
  else if (parser.is("HP", 3)) {
    meterHp.img.createFromFile(
        getResource(parser.getArg(1), Parser::EXT_IMAGE));

    meterHp.pos.x = parser.getArgInt(2);
    meterHp.pos.y = parser.getArgInt(3);
  }
  else if (parser.is("SUPER", 3)) {
    meterSuper.img.createFromFile(
        getResource(parser.getArg(1), Parser::EXT_IMAGE));

    meterSuper.pos.x = parser.getArgInt(2);
    meterSuper.pos.y = parser.getArgInt(3);
  }
  else if (parser.is("TAG", 3)) {
    meterTag.img.createFromFile(
        getResource(parser.getArg(1), Parser::EXT_IMAGE));

    meterTag.pos.x = parser.getArgInt(2);
    meterTag.pos.y = parser.getArgInt(3);
  }
  else if (parser.is("STUN", 3)) {
    meterStun.img.createFromFile(
        getResource(parser.getArg(1), Parser::EXT_IMAGE));

    meterStun.pos.x = parser.getArgInt(2);
    meterStun.pos.y = parser.getArgInt(3);
  }
  else if (parser.is("GUARD", 3)) {
    meterGuard.img.createFromFile(
        getResource(parser.getArg(1), Parser::EXT_IMAGE));

    meterGuard.pos.x = parser.getArgInt(2);
    meterGuard.pos.y = parser.getArgInt(3);
  }
  else if (parser.is("DPM", 3)) {
    meterDpm.img.createFromFile(
        getResource(parser.getArg(1), Parser::EXT_IMAGE));

    meterDpm.pos.x = parser.getArgInt(2);
    meterDpm.pos.y = parser.getArgInt(3);
  }
  else if (parser.is("STATIC", 2)) {
    staticImg.createFromFile(getResource(parser.getArg(1), Parser::EXT_IMAGE));
    staticSnd = getResourceT<audio::Sound>(parser.getArg(2));
  }
  else if (parser.is("FADE", 2)) {
    fadeinSnd = getResourceT<audio::Sound>(parser.getArg(1));
    fadeoutSnd = getResourceT<audio::Sound>(parser.getArg(2));
  }
  else if (parser.is("ROUND_SPLASH", 5)) {
    for (int i = 0; i < 5; i++) {
      round_splash[i].createFromFile(
          getResource(parser.getArg(i + 1), Parser::EXT_IMAGE));
    }
  }
  else if (parser.is("ROUND_HUD", 7)) {
    for (int i = 0; i < 5; i++) {
      round_hud[i].createFromFile(
          getResource(parser.getArg(i + 1), Parser::EXT_IMAGE));
    }
    x_round_hud = parser.getArgInt(6);
    y_round_hud = parser.getArgInt(7);
  }
  else if (parser.is("KO", 3)) {
    for (int i = 0; i < 3; i++) {
      ko[i].createFromFile(
          getResource(parser.getArg(i + 1), Parser::EXT_IMAGE));
    }
  }
  else if (parser.is("PORTRAIT_POS", 2)) {
    portraitPos.x = parser.getArgInt(1);
    portraitPos.y = parser.getArgInt(2);
  }
  else if (parser.is("COMBO", 3)) {
    combo = getResourceT<Font>(parser.getArg(1));
    comboLeft.createFromFile(getResource(parser.getArg(2), Parser::EXT_IMAGE));
    comboRight.createFromFile(getResource(parser.getArg(3), Parser::EXT_IMAGE));
  }
  else if (parser.is("WIN", 3)) {
    win.createFromFile(getResource(parser.getArg(1), Parser::EXT_IMAGE));
    win_font = getResourceT<Font>(parser.getArg(2));
    win_bgm.createFromFile("",
                           getResource(parser.getArg(3), Parser::EXT_MUSIC));
  }
  else if (parser.is("WIN_ORBS", 5)) {
    orb_null.createFromFile(getResource(parser.getArg(1), Parser::EXT_IMAGE));
    orb_win.createFromFile(getResource(parser.getArg(2), Parser::EXT_IMAGE));
    orb_draw.createFromFile(getResource(parser.getArg(3), Parser::EXT_IMAGE));

    orb_pos.x = parser.getArgInt(4);
    orb_pos.y = parser.getArgInt(5);
  }
  else {
    Scene::parseLine(parser);
  }
}

void scene::Fight::parseJSON(const nlohmann::ordered_json &j_obj) {
  if (j_obj.contains("hud")) {
    hud.createFromFile(
        getResource(j_obj["hud"].at("image"), Parser::EXT_IMAGE));
  }
  if (j_obj.contains("hud_tag")) {
    hud_tag.createFromFile(
        getResource(j_obj["hud_tag"].at("image"), Parser::EXT_IMAGE));
  }
  if (j_obj.contains("portraits")) {
    portraits.createFromFile(
        getResource(j_obj["portraits"].at("image"), Parser::EXT_IMAGE));
  }
  if (j_obj.contains("portraits_tag")) {
    portraits_tag.createFromFile(
        getResource(j_obj["portraits_tag"].at("image"), Parser::EXT_IMAGE));
  }
  if (j_obj.contains("timer")) {
    timer.createFromFile(
        getResource(j_obj["timer"].at("image"), Parser::EXT_IMAGE));
    timer_font = getResourceT<Font>(j_obj["timer"].at("font"));
  }
  if (j_obj.contains("shine")) {
    shine.createFromFile(
        getResource(j_obj["shine"].at("image"), Parser::EXT_IMAGE));
  }
  if (j_obj.contains("shine_tag")) {
    shine_tag.createFromFile(
        getResource(j_obj["shine_tag"].at("image"), Parser::EXT_IMAGE));
  }
  if (j_obj.contains("hp")) {
    meterHp.img.createFromFile(
        getResource(j_obj["hp"].at("image"), Parser::EXT_IMAGE));

    meterHp.pos.x = j_obj["hp"].at("pos").at("x");
    meterHp.pos.y = j_obj["hp"].at("pos").at("y");
  }
  if (j_obj.contains("super")) {
    meterSuper.img.createFromFile(
        getResource(j_obj["super"].at("image"), Parser::EXT_IMAGE));

    meterSuper.pos.x = j_obj["super"].at("pos").at("x");
    meterSuper.pos.y = j_obj["super"].at("pos").at("y");
  }
  if (j_obj.contains("tag")) {
    meterTag.img.createFromFile(
        getResource(j_obj["tag"].at("image"), Parser::EXT_IMAGE));

    meterTag.pos.x = j_obj["tag"].at("pos").at("x");
    meterTag.pos.y = j_obj["tag"].at("pos").at("y");
  }
  if (j_obj.contains("stun")) {
    meterStun.img.createFromFile(
        getResource(j_obj["stun"].at("image"), Parser::EXT_IMAGE));

    meterStun.pos.x = j_obj["stun"].at("pos").at("x");
    meterStun.pos.y = j_obj["stun"].at("pos").at("y");
  }
  if (j_obj.contains("guard")) {
    meterGuard.img.createFromFile(
        getResource(j_obj["guard"].at("image"), Parser::EXT_IMAGE));

    meterGuard.pos.x = j_obj["guard"].at("pos").at("x");
    meterGuard.pos.y = j_obj["guard"].at("pos").at("y");
  }
  if (j_obj.contains("dpm")) {
    meterDpm.img.createFromFile(
        getResource(j_obj["dpm"].at("image"), Parser::EXT_IMAGE));

    meterDpm.pos.x = j_obj["dpm"].at("pos").at("x");
    meterDpm.pos.y = j_obj["dpm"].at("pos").at("y");
  }
  if (j_obj.contains("static")) {
    staticImg.createFromFile(
        getResource(j_obj["static"].at("image"), Parser::EXT_IMAGE));
    staticSnd = getResourceT<audio::Sound>(j_obj["static"].at("sound"));
  }
  if (j_obj.contains("fade")) {
    fadeinSnd = getResourceT<audio::Sound>(j_obj["fade"].at("sound").at("in"));
    fadeoutSnd =
        getResourceT<audio::Sound>(j_obj["fade"].at("sound").at("out"));
  }
  if (j_obj.contains("round_splash")) {
    for (int i = 0; i < 5; i++) {
      round_splash[i].createFromFile(getResource(
          j_obj["round_splash"].at("image").at(i), Parser::EXT_IMAGE));
    }
  }
  if (j_obj.contains("round_hud")) {
    for (int i = 0; i < 5; i++) {
      round_hud[i].createFromFile(
          getResource(j_obj["round_hud"].at("image").at(i), Parser::EXT_IMAGE));
    }
    x_round_hud = j_obj["round_hud"].at("pos").at("x");
    y_round_hud = j_obj["round_hud"].at("pos").at("y");
  }
  if (j_obj.contains("ko")) {
    ko[0].createFromFile(
        getResource(j_obj["ko"].at("image").at("ko"), Parser::EXT_IMAGE));
    ko[1].createFromFile(
        getResource(j_obj["ko"].at("image").at("timeout"), Parser::EXT_IMAGE));
    ko[2].createFromFile(
        getResource(j_obj["ko"].at("image").at("draw"), Parser::EXT_IMAGE));
  }
  if (j_obj.contains("portrait_pos")) {
    portraitPos.x = j_obj["portrait_pos"].at("x");
    portraitPos.y = j_obj["portrait_pos"].at("y");
  }
  if (j_obj.contains("combo")) {
    combo = getResourceT<Font>(j_obj["combo"].at("font"));
    comboLeft.createFromFile(
        getResource(j_obj["combo"].at("image").at("left"), Parser::EXT_IMAGE));
    comboRight.createFromFile(
        getResource(j_obj["combo"].at("image").at("right"), Parser::EXT_IMAGE));
  }
  if (j_obj.contains("win")) {
    win.createFromFile(
        getResource(j_obj["win"].at("image"), Parser::EXT_IMAGE));
    win_font = getResourceT<Font>(j_obj["win"].at("font"));
    win_bgm.createFromFile(
        "", getResource(j_obj["win"].at("bgm"), Parser::EXT_MUSIC));
  }
  if (j_obj.contains("win_orbs")) {
    orb_null.createFromFile(getResource(
        j_obj["win_orbs"].at("image").at("null"), Parser::EXT_IMAGE));
    orb_win.createFromFile(getResource(j_obj["win_orbs"].at("image").at("win"),
                                       Parser::EXT_IMAGE));
    orb_draw.createFromFile(getResource(
        j_obj["win_orbs"].at("image").at("draw"), Parser::EXT_IMAGE));

    orb_pos.x = j_obj["win_orbs"].at("pos").at("x");
    orb_pos.y = j_obj["win_orbs"].at("pos").at("y");
  }
  Scene::parseJSON(j_obj);
}

void scene::Fight::think() {
  const util::Vectorf &m_pos = madotsuki.getcrPos();
  const util::Vectorf &p_pos = poniko.getcrPos();

  if (frameShakeTimer) {
    cameraShake.x = (util::roll(frameShakeTimer * 2)) - frameShakeTimer;
    cameraShake.y = (util::roll(frameShakeTimer * 2)) - frameShakeTimer;
    frameShakeTimer--;
  }
  else {
    cameraShake.x = cameraShake.y = 0;
  }

  idealCameraPos.x = (m_pos.x + p_pos.x) / 2;
  idealCameraPos.y = (m_pos.y + p_pos.y) / 3 - 30;

  int heightAbs = STAGE->getCamHeight();
  int widthAbs = STAGE->getCamWidth();
  if (idealCameraPos.y < 0) {
    idealCameraPos.y = 0;
  }
  if (idealCameraPos.y > heightAbs - sys::WINDOW_HEIGHT) {
    idealCameraPos.y = heightAbs - sys::WINDOW_HEIGHT;
  }

  if (idealCameraPos.x < widthAbs / -2 + sys::WINDOW_WIDTH / 2) {
    idealCameraPos.x = widthAbs / -2 + sys::WINDOW_WIDTH / 2;
  }
  else if (idealCameraPos.x > widthAbs / 2 - sys::WINDOW_WIDTH / 2) {
    idealCameraPos.x = widthAbs / 2 - sys::WINDOW_WIDTH / 2;
  }

  cameraPos.x = (cameraPos.x * 0.8 + idealCameraPos.x * 0.2);
  cameraPos.y = (cameraPos.y * 0.8 + idealCameraPos.y * 0.2);

  cameraPos.x += cameraShake.x;
  cameraPos.y += cameraShake.y;

  if (m_pos.x < p_pos.x) {
    // if(madotsuki.inStandardState(STATE_STAND))
    if (madotsuki.isIdle()) {
      madotsuki.setDirection(game::RIGHT);
    }
    // if(poniko.inStandardState(STATE_STAND))
    if (poniko.isIdle()) {
      poniko.setDirection(game::LEFT);
    }
  }
  else if (m_pos.x > p_pos.x) {
    // if(madotsuki.inStandardState(STATE_STAND))
    if (madotsuki.isIdle()) {
      madotsuki.setDirection(game::LEFT);
    }
    // if(poniko.inStandardState(STATE_STAND))
    if (poniko.isIdle()) {
      poniko.setDirection(game::RIGHT);
    }
  }

  STAGE->think();

  madotsuki.think();
  poniko.think();

  if (!framePauseTimer) {

    madotsuki.interact(&poniko);
    poniko.interact(&madotsuki);

    for (int i = 0; i < game::MAX_PROJECTILES; i++) {
      game::Projectile *m_proj = madotsuki.getProjectileAt(i);
      if (m_proj->getState() != game::STATE_NONE) {
        m_proj->interact(&poniko);
      }
      game::Projectile *p_proj = poniko.getProjectileAt(i);
      if (p_proj->getState() != game::STATE_NONE) {
        p_proj->interact(&madotsuki);
      }
    }
  }
  else {
    framePauseTimer--;
  }

  Scene::think();

  if (gametype == GAMETYPE_TRAINING) {
    ko_player = 0;
    timer_ko = 0;
    timer_round_in = 0;
    timer_round_out = 0;
    game_timer = 0;

    if (!bgmPlaying) {
      STAGE->bgmPlay();
      bgmPlaying = true;
    }
  }

  if (winner) {
    if (input(game::INPUT_B)) {
      setScene(SCENE_SELECT);
    }
    else if (input(game::INPUT_A)) {
      reset();
    }
  }
  else {
    // Decrement timer
    if (game_timer && !timer_ko && !timer_round_in && !timer_round_out) {
      if (!--game_timer) {
        timer_ko = 1 * sys::FPS;
        ko_type = 1;
        int m_hp = madotsuki.getHp();
        int p_hp = poniko.getHp();
        if (m_hp == p_hp) {
          ko_player = 3;
        }
        else if (m_hp < p_hp) {
          ko_player = 1;
        }
        else {
          ko_player = 2;
        }

        if (madotsuki.getFlags() & game::F_ON_GROUND && madotsuki.isDashing()) {
          if (madotsuki.inStandardState(game::STATE_DASH_FORWARD)) {
            madotsuki.setStandardState(game::STATE_DASH_FORWARD_END);
          }
          if (madotsuki.inStandardState(game::STATE_DASH_BACK)) {
            madotsuki.setStandardState(game::STATE_DASH_BACK_END);
          }
        }
        if (poniko.getFlags() & game::F_ON_GROUND && poniko.isDashing()) {
          if (poniko.inStandardState(game::STATE_DASH_FORWARD)) {
            poniko.setStandardState(game::STATE_DASH_FORWARD_END);
          }
          if (poniko.inStandardState(game::STATE_DASH_BACK)) {
            poniko.setStandardState(game::STATE_DASH_BACK_END);
          }
        }
      }
    }

    // Combo counters
    // LEFT
    int m_comboCounter = madotsuki.getComboCounter();
    if (!ko_player) {
      if (m_comboCounter) {
        if (m_comboCounter == 1) {
          comboLeftOff = 0;
          comboLeftTimer = 0;
        }
        else if (m_comboCounter >= 2) {
          if (comboLeftLast < m_comboCounter) {
            comboLeftTimer = sys::FPS;
          }
          else if (comboLeftLast > m_comboCounter) {
            comboLeftOff = 0;
            comboLeftTimer = sys::FPS;
          }
        }
        comboLeftLast = m_comboCounter;
      }
    }

    if (comboLeftTimer) {
      comboLeftTimer--;
      unsigned int comboLeftW = comboLeft.getW();
      if (comboLeftOff < comboLeftW) {
        comboLeftOff += 16;
      }
      if (comboLeftOff > comboLeftW) {
        comboLeftOff = comboLeftW;
      }
    }
    else if (comboLeftOff && (m_comboCounter < 2 || ko_player)) {
      if (comboLeftOff > 0) {
        comboLeftOff -= 16;
      }
      if (comboLeftOff <= 0) {
        comboLeftOff = 0;
        comboLeftLast = 0;
      }
    }

    // RIGHT
    int p_comboCounter = poniko.getComboCounter();
    if (p_comboCounter > 1 && !ko_player) {
      if (comboRightLast < p_comboCounter) {
        comboRightTimer = sys::FPS;
      }
      else if (comboRightLast > p_comboCounter) {
        comboRightOff = 0;
        comboRightTimer = sys::FPS;
      }
      comboRightLast = p_comboCounter;
    }

    if (comboRightTimer) {
      comboRightTimer--;
      unsigned int comboRightW = comboRight.getW();
      if (comboRightOff < comboRightW) {
        comboRightOff += 16;
      }
      if (comboRightOff > comboRightW) {
        comboRightOff = comboRightW;
      }
    }
    else if (comboRightOff && (p_comboCounter < 2 || ko_player)) {
      if (comboRightOff > 0) {
        comboRightOff -= 16;
      }
      if (comboRightOff <= 0) {
        comboRightOff = 0;
        comboRightLast = 0;
      }
    }

    // ROUND INTROS
    if (!Options::isEpilepsy() && (timer_round_out || timer_round_in)) {
      if (!timer_flash && !util::roll(64)) {
        staticSnd->play();
        timer_flash = 5;
        staticImg.draw<renderer::Texture2DRenderer>(0, 0);
      }
    }
    if (timer_round_in == (int)(4.0 * sys::FPS)) {
      madotsuki.reset();
      poniko.reset();
      cameraPos.x = 0;
      cameraPos.y = 0;
      idealCameraPos.x = 0;
      idealCameraPos.y = 0;
      fadeinSnd->play();
    }
    if (timer_round_out == (int)(1.5 * sys::FPS)) {
      fadeoutSnd->play();
    }

    if (timer_round_in == (int)(1.4 * sys::FPS) && !bgmPlaying) {
      STAGE->bgmPlay();
      bgmPlaying = true;
    }

    if (timer_round_in) {
      timer_round_in--;
    }
    if (timer_round_out) {
      timer_round_out--;
      if (timer_round_out == (int)(3.8 * sys::FPS)) {
        uint32_t m_flags = madotsuki.getFlags();
        uint32_t p_flags = poniko.getFlags();
        if (ko_player == 2) {
          if (!(p_flags & game::F_DEAD)) {
            poniko.setStandardState(game::STATE_DEFEAT);
          }
          madotsuki.setStandardState(game::STATE_VICTORY);
          win_types[0][wins[0]++] = 0;
        }
        else if (ko_player == 1) {
          if (!(m_flags & game::F_DEAD)) {
            madotsuki.setStandardState(game::STATE_DEFEAT);
          }
          poniko.setStandardState(game::STATE_VICTORY);
          win_types[1][wins[1]++] = 0;
        }
        else if (ko_player == 3) {
          if (ko_type != 2) {
            timer_round_out = 0;
            timer_ko = 1 * sys::FPS;
            ko_type = 2;
          }
          else {
            if (!(m_flags & game::F_DEAD)) {
              madotsuki.setStandardState(game::STATE_DEFEAT);
            }
            if (!(p_flags & game::F_DEAD)) {
              poniko.setStandardState(game::STATE_DEFEAT);
            }
            win_types[0][wins[0]++] = 1;
            win_types[1][wins[1]++] = 1;
            ko_type = 0;
          }
        }
      }
      else if (!timer_round_out) {
        // See if someone's won
        int winReq = Options::getWins();
        if (wins[0] >= winReq || wins[1] >= winReq) {
          // Count up the wins
          int wins_p1 = 0;
          int wins_p2 = 0;
          for (int i = 0; i < wins[0]; i++)
            if (win_types[0][i] == 0) {
              wins_p1++;
            }
          for (int i = 0; i < wins[1]; i++)
            if (win_types[1][i] == 0) {
              wins_p2++;
            }

          if (wins_p1 > wins_p2) {
            winner = 1;
          }
          else if (wins_p1 < wins_p2) {
            winner = 2;
          }
          else {
            winner = 3;
          }
        }

        if (winner) {
          win_bgm.play();
        }
        timer_round_in = 4.0 * sys::FPS;
        ko_player = 0;
        game_timer = Options::getTime() * sys::FPS - 1;
        if (game_timer < 0) {
          game_timer = 0;
        }
        round++;
      }
    }
    if (timer_ko) {
      timer_ko--;
    }

    if (ko_player && !timer_ko && !timer_round_out) {
      // Make sure everyone's still
      bool _condition = false;
      uint32_t m_flags = madotsuki.getFlags();
      uint32_t p_flags = poniko.getFlags();
      if (ko_type == 0) {
        if (ko_player == 2) {
          _condition = madotsuki.isIdle() && (m_flags & game::F_ON_GROUND) &&
                       (poniko.inStandardState(game::STATE_PRONE) ||
                        poniko.inStandardState(game::STATE_ON_BACK));
        }
        else if (ko_player == 1) {
          _condition = poniko.isIdle() && (p_flags & game::F_ON_GROUND) &&
                       (madotsuki.inStandardState(game::STATE_PRONE) ||
                        madotsuki.inStandardState(game::STATE_ON_BACK));
        }
      }
      else if (ko_type == 1) {
        _condition = poniko.isIdle() && (p_flags & game::F_ON_GROUND) &&
                     madotsuki.isIdle() && (m_flags & game::F_ON_GROUND);
      }
      else if (ko_type == 2) {
        _condition = true;
      }
      if (_condition) {
        timer_round_out = 4 * sys::FPS;
      }
    }
  }
}

void scene::Fight::draw() const {
  // From main.cpp
  STAGE->draw(false);

  madotsuki.drawSpecial();
  poniko.drawSpecial();

  if (Stage::getStageIndex() != 3) {
    madotsuki.draw(true);
    poniko.draw(true);
  }

  // Which order do we draw these in?
  if (madotsuki.getDrawPriorityFrame() < poniko.getDrawPriorityFrame()) {
    madotsuki.draw(false);
    poniko.draw(false);
  }
  else {
    poniko.draw(false);
    madotsuki.draw(false);
  }

  // Draw projectiles
  for (int i = 0; i < game::MAX_PROJECTILES; i++) {
    const game::Projectile *m_proj = madotsuki.getProjectileAt(i);
    if (m_proj->getState() != game::STATE_NONE) {
      m_proj->draw();
    }
    const game::Projectile *p_proj = poniko.getProjectileAt(i);
    if (p_proj->getState() != game::STATE_NONE) {
      p_proj->draw();
    }
  }

  STAGE->draw(true);

  effect::draw();

  Scene::draw();

  const game::Fighter *m_fighter = madotsuki.getcFighter();
  const game::Fighter *p_fighter = poniko.getcFighter();
  if (winner) {
    win.draw<renderer::Texture2DRenderer>(0, 0);

    char _b_sz[256];
    if (winner == 3) {
      strcpy(_b_sz, "Draw!");
    }
    else {
      sprintf(_b_sz, "Player %d wins!", winner);
    }
    win_font->drawText(32, sys::FLIP(32), _b_sz);
  }
  else {
    hud.draw<renderer::Texture2DRenderer>(0, 0);
    hud.draw<renderer::Texture2DRenderer>(sys::WINDOW_WIDTH - hud.getW(), 0,
                                          true);

    // DRAW METERS

    meterHp.draw(madotsuki.getHp() / (float)madotsuki.getMaxHp(), false, false);
    meterHp.draw(poniko.getHp() / (float)poniko.getMaxHp(), true, false);

    meterSuper.draw(madotsuki.getSuper() / (float)game::SUPER_MAX, false,
                    false);
    meterSuper.draw(poniko.getSuper() / (float)game::SUPER_MAX, true, false);

    // meterTag.draw(1, false, false);
    // meterTag.draw(1, true, false);

    meterStun.draw(1, false, false);
    meterStun.draw(1, true, false);

    meterGuard.draw(1, false, true);
    meterGuard.draw(1, true, true);

    meterDpm.draw(1, false, false);
    meterDpm.draw(1, true, false);

    unsigned int shineW = shine.getW();
    portraits.draw<renderer::Texture2DRenderer>(0, 0);
    portraits.draw<renderer::Texture2DRenderer>(sys::WINDOW_WIDTH - shineW, 0,
                                                true);

    timer.draw<renderer::Texture2DRenderer>(0, 0);

    if (FIGHT->gametype != GAMETYPE_TRAINING) {
      // Round orbs
      for (int i = 0; i < Options::getWins(); i++) {
        int x = orb_pos.x - i * 18;
        if (i < wins[0]) {
          if (win_types[0][i]) {
            orb_draw.draw<renderer::Texture2DRenderer>(x, orb_pos.y);
          }
          else {
            orb_win.draw<renderer::Texture2DRenderer>(x, orb_pos.y);
          }
        }
        else {
          orb_null.draw<renderer::Texture2DRenderer>(x, orb_pos.y);
        }

        if (i < wins[1]) {
          if (win_types[0][i]) {
            orb_draw.draw<renderer::Texture2DRenderer>(
                sys::WINDOW_WIDTH - x - 18, orb_pos.y);
          }
          else {
            orb_win.draw<renderer::Texture2DRenderer>(
                sys::WINDOW_WIDTH - x - 18, orb_pos.y);
          }
        }
        else {
          orb_null.draw<renderer::Texture2DRenderer>(sys::WINDOW_WIDTH - x - 18,
                                                     orb_pos.y);
        }
      }

      // Draw timer
      char b_timer_text[8];
      if (game_timer) {
        sprintf(b_timer_text, "%02d", (game_timer / sys::FPS) + 1);
      }
      else {
        strcpy(b_timer_text, "00");
      }
      int w_timer_text = timer_font->getTextWidth(b_timer_text);
      timer_font->drawText((sys::WINDOW_WIDTH - w_timer_text) / 2, 30,
                           b_timer_text);
    }

    shine.draw<renderer::Texture2DRenderer>(0, 0);
    shine.draw<renderer::Texture2DRenderer>(sys::WINDOW_WIDTH - shineW, 0,
                                            true);

    round_hud[round].draw<renderer::Texture2DRenderer>(x_round_hud,
                                                       y_round_hud);

    // Draw combo counters
    // LEFT
    if (comboLeftOff) {
      comboLeft.draw<renderer::Texture2DRenderer>(
          comboLeftOff - comboLeft.getW(), 131);
      char buff[8];
      sprintf(buff, "%d", comboLeftLast);
      int w = combo->getTextWidth(buff);
      combo->drawText(comboLeftOff - w / 2 - 100, 131 + 35, buff);
    }

    // RIGHT
    if (comboRightOff) {
      comboRight.draw<renderer::Texture2DRenderer>(
          sys::WINDOW_WIDTH - comboRightOff, 131);
      char buff[8];
      sprintf(buff, "%d", comboRightLast);
      int w = combo->getTextWidth(buff);
      combo->drawText(sys::WINDOW_WIDTH - comboRightOff - w / 2 + 100, 131 + 35,
                      buff);
    }

    // Draw character portraits
    bool shader_support = graphics::hasShaderSupport();
    if (shader_support) {
      graphics::setPalette(m_fighter->getcrPalettes()[madotsuki.getPalette()],
                           1.0f, 0.0f, 0.0f, 0.0f, 0.0f);
    }
    m_fighter->getcImagePortraitUI()->draw<renderer::FighterRenderer>(
        portraitPos.x, portraitPos.y);
    if (shader_support) {
      graphics::setPalette(p_fighter->getcrPalettes()[poniko.getPalette()],
                           1.0f, 0.0f, 0.0f, 0.0f, 0.0f);
    }
    else {
      renderer::FighterRenderer::setColor(150 / 255.0f, 150 / 255.0f,
                                          150 / 255.0f);
      renderer::FighterRenderer::setAlpha(1.0f);
    }
    const Image *p_portraitUI = p_fighter->getcImagePortraitUI();
    p_portraitUI->draw<renderer::FighterRenderer>(
        sys::WINDOW_WIDTH - portraitPos.x - p_portraitUI->getW(), portraitPos.y,
        true);
    if (shader_support) {
      renderer::ShaderProgram::unuse();
    }

    // Draw round transitions
    if (timer_flash) {
      renderer::Texture2DRenderer::setColor(180 / 255.0f, 120 / 255.0f,
                                            190 / 255.0f, timer_flash / 5.0f);
      staticImg.draw<renderer::Texture2DRenderer>(
          -util::roll(sys::WINDOW_WIDTH), -util::roll(sys::WINDOW_HEIGHT));
      timer_flash--;
    }

    if (timer_round_out) {
      float alpha = 0.0f;
      if (timer_round_out < 0.5 * sys::FPS) {
        alpha = 1.0f;
      }
      else if (timer_round_out < 1.5 * sys::FPS) {
        alpha = 1.0 - ((timer_round_out - 0.5 * sys::FPS) / (1.0 * sys::FPS));
      }
      renderer::Texture2DRenderer::setColor(180 / 255.0f, 120 / 255.0f,
                                            190 / 255.0f, alpha);
      staticImg.draw<renderer::Texture2DRenderer>(
          -util::roll(sys::WINDOW_WIDTH), -util::roll(sys::WINDOW_HEIGHT));
    }

    if (timer_round_in) {
      if (timer_round_in > 1.0 * sys::FPS) {
        renderer::Texture2DRenderer::setColor(
            180 / 255.0f, 120 / 255.0f, 190 / 255.0f,
            ((timer_round_in - 1.0 * sys::FPS) / (3.0 * sys::FPS)));
        staticImg.draw<renderer::Texture2DRenderer>(
            -util::roll(sys::WINDOW_WIDTH), -util::roll(sys::WINDOW_HEIGHT));
      }

      if (timer_round_in <= 1.4 * sys::FPS) {
        unsigned int splashW = round_splash[round].getW();
        unsigned int splashH = round_splash[round].getH();
        if (timer_round_in > 1.3 * sys::FPS) {
          float scalar =
              (timer_round_in - 1.3 * sys::FPS) / (0.1 * sys::FPS) + 1.0;
          graphics::setScale(scalar);
          round_splash[round].draw<renderer::Texture2DRenderer>(
              sys::WINDOW_WIDTH / 2 - splashW * scalar / 2 - util::roll(10, 30),
              sys::WINDOW_HEIGHT / 2 - splashH * scalar / 2 -
                  util::roll(10, 30));
        }
        else if (timer_round_in < 0.1 * sys::FPS) {
          float scalar = timer_round_in / (0.1 * sys::FPS);
          renderer::Texture2DRenderer::setColor(1.0f, 1.0f, 1.0f, scalar);
          round_splash[round].draw<renderer::Texture2DRenderer>(
              sys::WINDOW_WIDTH / 2 - splashW / 2,
              sys::WINDOW_HEIGHT / 2 - splashH / 2);
          renderer::Texture2DRenderer::setColor(1.0f, 1.0f, 1.0f, scalar);
          scalar = 1.0f - scalar + 1.0f;
          float xscalar = 1 / scalar;
          graphics::setScale(xscalar, scalar);
          round_splash[round].draw<renderer::Texture2DRenderer>(
              sys::WINDOW_WIDTH / 2 - splashW * xscalar / 2 -
                  util::roll(10, 30),
              sys::WINDOW_HEIGHT / 2 - splashH * scalar / 2 -
                  util::roll(10, 30));
        }
        else {
          round_splash[round].draw<renderer::Texture2DRenderer>(
              sys::WINDOW_WIDTH / 2 - splashW / 2 - util::roll(5, 15),
              sys::WINDOW_HEIGHT / 2 - splashH / 2 - util::roll(5, 15));
        }
      }
    }

    if (timer_ko) {
      unsigned int koImgW = ko[ko_type].getW();
      unsigned int koImgH = ko[ko_type].getH();
      if (timer_ko > 0.8 * sys::FPS) {
        float scalar = (timer_ko - 0.8 * sys::FPS) / (0.1 * sys::FPS) + 1.0;
        graphics::setScale(scalar);
        ko[ko_type].draw<renderer::Texture2DRenderer>(
            sys::WINDOW_WIDTH / 2 - koImgW * scalar / 2 - util::roll(10, 30),
            sys::WINDOW_HEIGHT / 2 - koImgH * scalar / 2 - util::roll(10, 30));
      }
      else if (timer_ko < 0.1 * sys::FPS) {
        float scalar = timer_ko / (0.1 * sys::FPS);
        renderer::Texture2DRenderer::setColor(1.0f, 1.0f, 1.0f, scalar);
        ko[ko_type].draw<renderer::Texture2DRenderer>(
            sys::WINDOW_WIDTH / 2 - koImgW / 2,
            sys::WINDOW_HEIGHT / 2 - koImgH / 2);
        renderer::Texture2DRenderer::setColor(1.0f, 1.0f, 1.0f, scalar);
        scalar = 1.0f - scalar + 1.0f;
        float xscalar = 1 / scalar;
        graphics::setScale(xscalar, scalar);
        ko[ko_type].draw<renderer::Texture2DRenderer>(
            sys::WINDOW_WIDTH / 2 - koImgW * xscalar / 2 - util::roll(10, 30),
            sys::WINDOW_HEIGHT / 2 - koImgH * scalar / 2 - util::roll(10, 30));
      }
      else {
        ko[ko_type].draw<renderer::Texture2DRenderer>(
            sys::WINDOW_WIDTH / 2 - koImgW / 2 - util::roll(5, 15),
            sys::WINDOW_HEIGHT / 2 - koImgH / 2 - util::roll(5, 15));
      }
    }
  }

  // From main.cpp
  const util::Vectorf &m_pos = madotsuki.getcrPos();
  reinterpret_cast<Select *>(getSceneFromIndex(SCENE_SELECT))
      ->drawEffect(0, m_fighter->getGroup(), m_pos.x,
                   m_pos.y + m_fighter->getHeight(), true);
}

void scene::Fight::reset() {
  ko_player = 0;
  ko_type = 0;

  timer_flash = 0;
  timer_round_in = 4.0 * sys::FPS;
  timer_round_out = 0;
  timer_ko = 0;

  round = 0;
  wins[0] = 0;
  wins[1] = 0;
  winner = 0;

  comboLeftOff = comboRightOff = 0;
  comboLeftLast = comboRightLast = 0;
  comboLeftTimer = comboRightTimer = 0;

  game_timer = Options::getTime() * sys::FPS - 1;
  if (game_timer < 0) {
    game_timer = 0;
  }

  bgmPlaying = false;
}

void scene::Fight::knockout(int player) {
  if (ko_player) {
    return;
  }
  ko_player = player + 1;
  timer_ko = 1 * sys::FPS;
  ko_type = 0;

  if (madotsuki.getFlags() & game::F_ON_GROUND && madotsuki.isDashing()) {
    if (madotsuki.inStandardState(game::STATE_DASH_FORWARD)) {
      madotsuki.setStandardState(game::STATE_DASH_FORWARD_END);
    }
    if (madotsuki.inStandardState(game::STATE_DASH_BACK)) {
      madotsuki.setStandardState(game::STATE_DASH_BACK_END);
    }
  }
  if (poniko.getFlags() & game::F_ON_GROUND && poniko.isDashing()) {
    if (poniko.inStandardState(game::STATE_DASH_FORWARD)) {
      poniko.setStandardState(game::STATE_DASH_FORWARD_END);
    }
    if (poniko.inStandardState(game::STATE_DASH_BACK)) {
      poniko.setStandardState(game::STATE_DASH_BACK_END);
    }
  }
}

int scene::Fight::getTimerRoundIn() const { return timer_round_in; }
int scene::Fight::getTimerRoundOut() const { return timer_round_out; }
int scene::Fight::getTimerKO() const { return timer_ko; }
int scene::Fight::getKOPlayer() const { return ko_player; }
int scene::Fight::getRound() const { return round; }
int scene::Fight::getGameType() const { return gametype; }
void scene::Fight::setGameType(int gametype) { this->gametype = gametype; }
