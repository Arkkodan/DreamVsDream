#include "netplay.h"

#include "fight.h"
#include "scene.h"

#include "../../util/rng.h"
#include "../graphics.h"
#include "../network.h"
#include "../resource_manager.h"
#include "../shader_renderer/primitive_renderer.h"
#include "../shader_renderer/texture2D_renderer.h"

#include <cstring>

// NETPLAY
#ifndef NO_NETWORK
scene::Netplay::Netplay() : Scene("netplay") {
  choice = 0;

  flashTimer = 0;
  flashDir = 1;
  drawShake = 0;
  barPos = 0;
  waiting = false;

  port = net::DEFAULT_PORT;
  ip = net::DEFAULT_IP;

  sndOn = sndOff = sndConStart = sndConSuccess = nullptr;
  menuFont = nullptr;

  updateIp(false);
  updatePort(false);
}

scene::Netplay::~Netplay() {}

void scene::Netplay::think() {
  Scene::think();

  if (sys::getFrame() % 2) {
    drawShake = !drawShake;
  }

  if (!flashDir) {
    volatile bool isConnected = net::isConnected();
    switch (mode) {
    case net::MODE_NONE: {
      if (input(game::INPUT_A)) {
        sndSelect->play();
        if (choice) {
          mode = net::MODE_CLIENT;
        }
        else {
          mode = net::MODE_SERVER;
        }
        choice = 0;
      }
      else if (input(game::INPUT_B)) {
        sndOff->play();
        flashDir = -1;
        flashTimer = NET_FLASH_TIME + NET_FLASH_HOLD_TIME + NET_FADE_TIME / 2;
      }
      if (input(game::INPUT_UP | game::INPUT_DOWN)) {
        sndMenu->play();
        choice = !choice;
      }
    } break;

    case net::MODE_SERVER: {
      if (waiting) {
        if (input(game::INPUT_A)) {
          if (isConnected) {
            sndSelect->play();
            FIGHT->setGameType(Fight::GAMETYPE_VERSUS);
            setScene(SCENE_SELECT);
          }
        }
        else if (input(game::INPUT_B)) {
          if (!isConnected) {
            sndBack->play();
            bgm.play();
            waiting = false;
            net::stop();
          }
        }
      }
      else if (!digit) {
        if (input(game::INPUT_A)) {
          sndSelect->play();
          if (choice > 0) {
            digit = 1;
          }
          else {
            waiting = true;
            net::start(0, port);
            bgmWait.play();
          }
        }
        else if (input(game::INPUT_B)) {
          sndBack->play();
          mode = net::MODE_NONE;
          choice = 0;
        }
        if (input(game::INPUT_UP | game::INPUT_DOWN)) {
          sndMenu->play();
          choice = !choice;
        }
      }
      else {
        if (input(game::INPUT_A)) {
          // Save changes
          sndSelect->play();
          updatePort(true);
          digit = 0;
        }
        else if (input(game::INPUT_B)) {
          // Discard changes
          sndBack->play();
          updatePort(false);
          digit = 0;
        }
        if (input(game::INPUT_DIRMASK)) {
          sndMenu->play();
        }
        if (input(game::INPUT_UP)) {
          if (++portStr[digit - 1] > 9) {
            portStr[digit - 1] = 0;
          }
        }
        else if (input(game::INPUT_DOWN)) {
          if (--portStr[digit - 1] < 0) {
            portStr[digit - 1] = 9;
          }
        }
        else if (input(game::INPUT_LEFT)) {
          if (--digit < 1) {
            digit = 5;
          }
        }
        else if (input(game::INPUT_RIGHT)) {
          if (++digit > 5) {
            digit = 1;
          }
        }
      }
    } break;

    case net::MODE_CLIENT: {
      if (waiting) {
        if (input(game::INPUT_A)) {
          if (isConnected) {
            sndSelect->play();
            FIGHT->setGameType(Fight::GAMETYPE_VERSUS);
            setScene(SCENE_SELECT);
          }
        }
        else if (input(game::INPUT_B)) {
          if (!isConnected) {
            sndBack->play();
            bgm.play();
            waiting = false;
            net::stop();
          }
        }
      }
      else if (!digit) {
        if (input(game::INPUT_A)) {
          if (choice == 0) {
            waiting = true;
            audio::Music::stop();
            sndConStart->play();
            net::start(ip, port);
          }
          else if (choice == 3) {
            sndSelect->play();

            // Copy IP and port from clipboard
            std::string clipboard = sys::getClipboard();
            bool hasIP = false;
            std::array<uint8_t, 4> octets{0};
            int octet_index = -1;
            auto isDigit = [](char c) { return c >= '0' && c <= '9'; };
            auto it = clipboard.cbegin();
            auto end = clipboard.cend();

            // Find IP
            for (; it != end; /* Manually change it */) {
              octet_index++;
              if (!isDigit(*it)) {
                // Fail if it does not point to a digit
                break;
              }

              // Find octet
              bool validOctet = true;
              auto jt = it + 1;
              for (; jt != end; jt++) {
                char c = *jt;
                if (octet_index < 3 && c == '.') {
                  // Break when next '.' is found and is not last octet
                  break;
                }
                else if (octet_index == 3 && c == ':') {
                  // Break if ':' is found and is last octet
                  break;
                }
                else if (!isDigit(c)) {
                  // Break if non-digit it reached
                  if (octet_index < 3) {
                    // Fail if not last octet
                    validOctet = false;
                  }
                  break;
                }
              }
              if (jt == end && octet_index < 3) {
                // Fail if jt reaches end and is not last octet
                break;
              }
              if (!validOctet) {
                // Fail if not a valid octet
                break;
              }

              // Generate octet
              std::string octet_candidate(it, jt);
              int octet = std::strtol(octet_candidate.c_str(), nullptr, 10);
              if (octet >= 0 && octet < 256) {
                octets[octet_index] = static_cast<uint8_t>(octet);
                if (jt != end) {
                  it = jt + 1;
                }
                if (octet_index == 3) {
                  // Pass if all 4 octets have been generated
                  hasIP = true;
                  break;
                }
              }
              else {
                // Fail if "octet" is out of range
                break;
              }
            }

            if (hasIP) {
              ip = 0;
              for (size_t i = 0; i < octets.size(); i++) {
                ip |= octets[i] << (8 * i);
              }
              updateIp(false);

              // Find port
              if (it >= end || *(it - 1) != ':') {
                // Missing port info or garbage data
                port = net::DEFAULT_PORT;
              }
              else if (*(it - 1) == ':') {
                if (!isDigit(*it)) {
                  // Not a digit after ':'
                  port = net::DEFAULT_PORT;
                }
                else {
                  // Find port
                  auto jt = it + 1;
                  for (; jt != end; jt++) {
                    if (!isDigit(*jt)) {
                      break;
                    }
                  }
                  std::string port_candidate(it, jt);
                  long port_unsanitized =
                      std::strtol(port_candidate.c_str(), nullptr, 10);
                  if (port_unsanitized > 0 && port_unsanitized < 65536) {
                    port = static_cast<uint16_t>(port_unsanitized);
                  }
                  else {
                    // "Port" is out of range
                    port = net::DEFAULT_PORT;
                  }
                }
              }

              updatePort(false);
            }
          }
          else {
            sndSelect->play();
            digit = 1;
          }
        }
        else if (input(game::INPUT_B)) {
          sndBack->play();
          mode = net::MODE_NONE;
          choice = 1;
        }
        if (input(game::INPUT_UP)) {
          sndMenu->play();
          if (--choice < 0) {
            choice = 3;
          }
        }
        else if (input(game::INPUT_DOWN)) {
          sndMenu->play();
          if (++choice > 3) {
            choice = 0;
          }
        }
      }
      else {
        if (input(game::INPUT_A)) {
          // Save changes
          sndSelect->play();
          if (choice == 1) {
            updateIp(true);
          }
          else if (choice == 2) {
            updatePort(true);
          }
          digit = 0;
        }
        else if (input(game::INPUT_B)) {
          // Discard changes
          sndBack->play();
          if (choice == 1) {
            updateIp(false);
          }
          else if (choice == 2) {
            updatePort(false);
          }
          digit = 0;
        }
        if (input(game::INPUT_DIRMASK)) {
          sndMenu->play();
        }
        if (choice == 1) {
          if (input(game::INPUT_UP)) {
            if (++ipStr[digit - 1] > 9) {
              ipStr[digit - 1] = 0;
            }
          }
          else if (input(game::INPUT_DOWN)) {
            if (--ipStr[digit - 1] < 0) {
              ipStr[digit - 1] = 9;
            }
          }
          else if (input(game::INPUT_LEFT)) {
            if (--digit < 1) {
              digit = 12;
            }
          }
          else if (input(game::INPUT_RIGHT)) {
            if (++digit > 12) {
              digit = 1;
            }
          }
        }
        else if (choice == 2) {
          if (input(game::INPUT_UP)) {
            if (++portStr[digit - 1] > 9) {
              portStr[digit - 1] = 0;
            }
          }
          else if (input(game::INPUT_DOWN)) {
            if (--portStr[digit - 1] < 0) {
              portStr[digit - 1] = 9;
            }
          }
          else if (input(game::INPUT_LEFT)) {
            if (--digit < 1) {
              digit = 5;
            }
          }
          else if (input(game::INPUT_RIGHT)) {
            if (++digit > 5) {
              digit = 1;
            }
          }
        }
      }
    } break;
    }
  }

  if (!flashTimer && flashDir == 1) {
    sndOn->play();
  }

  flashTimer += flashDir;
  if (flashTimer < 0) {
    flashDir = 0;
    flashTimer = 0;
    setScene(SCENE_TITLE);
  }
  else if (flashTimer > NET_FLASH_TIME + NET_FLASH_HOLD_TIME + NET_FADE_TIME) {
    flashDir = 0;
    flashTimer = NET_FLASH_TIME + NET_FLASH_HOLD_TIME + NET_FADE_TIME;
  }

  if (flashTimer == NET_FLASH_TIME + NET_FLASH_HOLD_TIME) {
    if (flashDir == 1) {
      bgm.play();
      // graphics::setClearColor(20, 20, 20);
    }
    else {
      audio::Music::stop();
      // graphics::setClearColor(0, 0, 0);
    }
  }

  barPos--;
  if (barPos < -NET_BAR_SIZE) {
    barPos = sys::WINDOW_HEIGHT + NET_BAR_SIZE;
  }
}

void scene::Netplay::draw() const {
  Scene::draw();

  int speed = (flashDir + 1) / 2 + 1;

  if (flashTimer > NET_FLASH_TIME) {
    float xscale = 1.0f;
    if (flashTimer <
        NET_FLASH_TIME + NET_FLASH_HOLD_TIME + NET_FADE_TIME / speed) {
      float add = (1.0f - (flashTimer - NET_FLASH_TIME - NET_FLASH_HOLD_TIME) /
                              (float)(NET_FADE_TIME / speed));
      add = add * add * add;
      xscale += add;
    }

    graphics::setScale(NET_SCALE * xscale, NET_SCALE);
    imgLogo.draw<renderer::Texture2DRenderer>(
        static_cast<int>(sys::WINDOW_WIDTH / 2 -
                         imgLogo.getW() * NET_SCALE * xscale / 2),
        sys::WINDOW_HEIGHT / 4 - imgLogo.getH() * NET_SCALE / 2 + drawShake);

    // Main menu
    volatile bool isConnected = net::isConnected();
    volatile bool isRunning = net::isRunning();
    switch (mode) {
    case net::MODE_NONE: {
      if (menuFont->exists()) {
        Font::setScale(NET_SCALE * xscale, NET_SCALE);
        menuFont->drawText(static_cast<int>(sys::WINDOW_WIDTH / 2 -
                                            (8 * 8 * NET_SCALE * xscale / 2)),
                           sys::WINDOW_HEIGHT / 3 * 2 + drawShake, "Server");
        Font::setScale(NET_SCALE * xscale, NET_SCALE);
        menuFont->drawText(static_cast<int>(sys::WINDOW_WIDTH / 2 -
                                            (8 * 8 * NET_SCALE * xscale / 2)),
                           sys::WINDOW_HEIGHT / 3 * 2 + 32 + drawShake,
                           "Client");
      }
      graphics::setScale(xscale, NET_SCALE);
      imgCursor.draw<renderer::Texture2DRenderer>(
          static_cast<int>(sys::WINDOW_WIDTH / 2 -
                           (10 * 8 * NET_SCALE * xscale / 2)),
          sys::WINDOW_HEIGHT / 3 * 2 + 32 * choice + drawShake);
    } break;

    case net::MODE_SERVER: {
      if (waiting) {
        if (menuFont->exists()) {
          Font::setScale(NET_SCALE);
          if (isConnected) {
            menuFont->drawText(sys::WINDOW_WIDTH / 2 - (23 * 8 * NET_SCALE / 2),
                               sys::WINDOW_HEIGHT / 3 * 2 - 8 * NET_SCALE / 2 +
                                   drawShake,
                               "Connection established!");
          }
          else if (isRunning) {
            menuFont->drawText(sys::WINDOW_WIDTH / 2 - (10 * 8 * NET_SCALE / 2),
                               sys::WINDOW_HEIGHT / 3 * 2 - 8 * NET_SCALE / 2 +
                                   drawShake,
                               "Waiting...");
          }
          else {
            menuFont->drawText(sys::WINDOW_WIDTH / 2 - (18 * 8 * NET_SCALE / 2),
                               sys::WINDOW_HEIGHT / 3 * 2 - 8 * NET_SCALE / 2 +
                                   drawShake,
                               "An error occurred.");
          }
        }
      }
      else {
        if (menuFont->exists()) {
          Font::setScale(NET_SCALE);
          menuFont->drawText(sys::WINDOW_WIDTH / 2 - (8 * 8 * NET_SCALE / 2),
                             sys::WINDOW_HEIGHT / 3 * 2 + drawShake, "Start");
          Font::setScale(NET_SCALE);
          menuFont->drawText(sys::WINDOW_WIDTH / 2 - (8 * 8 * NET_SCALE / 2),
                             sys::WINDOW_HEIGHT / 3 * 2 + 32 + drawShake,
                             "Port:");
          for (int i = 0; i < 5; i++) {
            Font::setScale(NET_SCALE);
            menuFont->drawChar(sys::WINDOW_WIDTH / 2 + (8 * 2 * NET_SCALE / 2) +
                                   i * 8 * NET_SCALE,
                               sys::WINDOW_HEIGHT / 3 * 2 -
                                   ((i + 1 == digit) ? 4 : 0) + 32 + drawShake,
                               portStr[i] + '0');
          }
          if (!digit) {
            graphics::setScale(xscale, NET_SCALE);
            imgCursor.draw<renderer::Texture2DRenderer>(
                sys::WINDOW_WIDTH / 2 - (10 * 8 * NET_SCALE / 2),
                sys::WINDOW_HEIGHT / 3 * 2 + 32 * choice + drawShake);
          }
        }
      }
    } break;

    case net::MODE_CLIENT: {
      if (waiting) {
        if (menuFont->exists()) {
          Font::setScale(NET_SCALE);
          if (isConnected) {
            menuFont->drawText(sys::WINDOW_WIDTH / 2 - (23 * 8 * NET_SCALE / 2),
                               sys::WINDOW_HEIGHT / 3 * 2 - 8 * NET_SCALE / 2 +
                                   drawShake,
                               "Connection established!");
          }
          else if (isRunning) {
            menuFont->drawText(sys::WINDOW_WIDTH / 2 - (13 * 8 * NET_SCALE / 2),
                               sys::WINDOW_HEIGHT / 3 * 2 - 8 * NET_SCALE / 2 +
                                   drawShake,
                               "Connecting...");
          }
          else {
            menuFont->drawText(sys::WINDOW_WIDTH / 2 - (18 * 8 * NET_SCALE / 2),
                               sys::WINDOW_HEIGHT / 3 * 2 - 8 * NET_SCALE / 2 +
                                   drawShake,
                               "An error occurred.");
          }
        }
      }
      else {
        if (menuFont->exists()) {
          Font::setScale(NET_SCALE);
          menuFont->drawText(sys::WINDOW_WIDTH / 2 - (8 * 8 * NET_SCALE / 2),
                             sys::WINDOW_HEIGHT / 3 * 2 + drawShake, "Start");
          Font::setScale(NET_SCALE);
          menuFont->drawText(sys::WINDOW_WIDTH / 2 - (8 * 8 * NET_SCALE / 2),
                             sys::WINDOW_HEIGHT / 3 * 2 + 32 + drawShake,
                             "IP:");
          for (int i = 0; i < 4; i++) {
            for (int j = 0; j < 3; j++) {
              Font::setScale(NET_SCALE);
              menuFont->drawChar(
                  sys::WINDOW_WIDTH / 2 + (8 * 2 * NET_SCALE / 2) +
                      (i * 4 + j) * 8 * NET_SCALE,
                  sys::WINDOW_HEIGHT / 3 * 2 -
                      ((choice == 1 && (i * 3 + j) + 1 == digit) ? 4 : 0) + 32 +
                      drawShake,
                  ipStr[i * 3 + j] + '0');
            }
            if (i < 3) {
              Font::setScale(NET_SCALE);
              menuFont->drawChar(
                  sys::WINDOW_WIDTH / 2 + (8 * 2 * NET_SCALE / 2) +
                      (i * 4 + 3) * 8 * NET_SCALE,
                  sys::WINDOW_HEIGHT / 3 * 2 + 32 + drawShake, '.');
            }
          }
          Font::setScale(NET_SCALE);
          menuFont->drawText(sys::WINDOW_WIDTH / 2 - (8 * 8 * NET_SCALE / 2),
                             sys::WINDOW_HEIGHT / 3 * 2 + 32 * 2 + drawShake,
                             "Port:");
          for (int i = 0; i < 5; i++) {
            Font::setScale(NET_SCALE);
            menuFont->drawChar(sys::WINDOW_WIDTH / 2 + (8 * 2 * NET_SCALE / 2) +
                                   i * 8 * NET_SCALE,
                               sys::WINDOW_HEIGHT / 3 * 2 -
                                   ((choice == 2 && i + 1 == digit) ? 4 : 0) +
                                   32 * 2 + drawShake,
                               portStr[i] + '0');
          }
          if (!digit) {
            graphics::setScale(xscale, NET_SCALE);
            imgCursor.draw<renderer::Texture2DRenderer>(
                sys::WINDOW_WIDTH / 2 - (10 * 8 * NET_SCALE / 2),
                sys::WINDOW_HEIGHT / 3 * 2 + 32 * choice + drawShake);
          }
          Font::setScale(NET_SCALE);
          menuFont->drawText(sys::WINDOW_WIDTH / 2 - (8 * 8 * NET_SCALE / 2),
                             sys::WINDOW_HEIGHT / 3 * 2 + 32 * 3 + drawShake,
                             "Copy from clipboard");
        }
      }
    } break;
    }

    renderer::PrimitiveRenderer::setColor({1.0f, 1.0f, 1.0f, 0.1f});
    renderer::PrimitiveRenderer::setPosRect(
        0.0f, static_cast<float>(sys::WINDOW_WIDTH),
        static_cast<float>(barPos + NET_BAR_SIZE), static_cast<float>(barPos));
    // TODO: Replace with a renderer that can do gradients:
    // glColor4f(1.0f, 1.0f, 1.0f, 0.0f);
    // glVertex3f(sys::WINDOW_WIDTH, barPos, 0);
    // glVertex3f(0, barPos, 0);
    // glColor4f(1.0f, 1.0f, 1.0f, 0.1f);
    // glVertex3f(0, barPos + NET_BAR_SIZE, 0);
    // glVertex3f(sys::WINDOW_WIDTH, barPos + NET_BAR_SIZE, 0);
    renderer::PrimitiveRenderer::draw();

    renderer::ShaderProgram::unuse();

    graphics::setRender(Image::Render::ADDITIVE);
    imgStatic.draw<renderer::Texture2DRenderer>(
        -util::roll(sys::WINDOW_WIDTH), -util::roll(sys::WINDOW_HEIGHT));
    graphics::setRender(Image::Render::MULTIPLY);
    imgScanlines.draw<renderer::Texture2DRenderer>(0, 0);
  }

  if (flashDir) {
    int xoff = (flashTimer * flashTimer * flashTimer) / 5;
    int yoff = 1;
    if (flashTimer > NET_FLASH_TIME / 2) {
      yoff = flashTimer - NET_FLASH_TIME / 2;
      yoff = (yoff * yoff * yoff) / 5;
    }
    float alpha = 1.0f;
    if (flashTimer > NET_FLASH_TIME + NET_FLASH_HOLD_TIME) {
      xoff = sys::WINDOW_WIDTH;
      yoff = sys::WINDOW_WIDTH;
      alpha = 1.0f - (flashTimer - NET_FLASH_TIME - NET_FLASH_HOLD_TIME) /
                         (float)(NET_FADE_TIME / speed);
    }
    // glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    renderer::PrimitiveRenderer::setColor({1.0f, 1.0f, 1.0f, alpha});
    renderer::PrimitiveRenderer::setPosRect(
        static_cast<float>(sys::WINDOW_WIDTH / 2 - xoff),
        static_cast<float>(sys::WINDOW_WIDTH / 2 + xoff),
        static_cast<float>(sys::WINDOW_HEIGHT / 2 + yoff),
        static_cast<float>(sys::WINDOW_HEIGHT / 2 - yoff));
    renderer::PrimitiveRenderer::draw();
    renderer::ShaderProgram::unuse();
  }
}

void scene::Netplay::reset() {
  Scene::reset();
  choice = 0;
  mode = net::MODE_NONE;
  digit = 0;
  flashDir = 1;
  flashTimer = 0;
  barPos = sys::WINDOW_HEIGHT / 3 * 2;
  waiting = false;
}

void scene::Netplay::updateIp(bool toint) {
  if (toint) {
    ip = 0;
    for (int i = 0; i < 4; i++) {
      int n = 0;
      for (int j = 0; j < 3; j++) {
        n *= 10;
        n += ipStr[i * 3 + j];
      }
      if (n > 255) {
        n = 255;
      }
      ip |= (uint8_t)n << (8 * i);
    }
    if (!ip) {
      ip = net::DEFAULT_IP;
    }
    updateIp(false);
  }
  else {
    int n1 = ip;
    for (int i = 0; i < 4; i++) {
      int n = n1 & 0xFF;
      for (int j = 0; j < 3; j++) {
        ipStr[i * 3 + (2 - j)] = n % 10;
        n /= 10;
      }
      n1 >>= 8;
    }
  }
}

void scene::Netplay::updatePort(bool toint) {
  if (toint) {
    int newport = 0;
    for (int i = 0; i < 5; i++) {
      newport *= 10;
      newport += portStr[i];
    }
    if (newport > 65535) {
      newport = 65535;
    }
    if (newport == 0) {
      newport = net::DEFAULT_PORT;
    }
    port = newport;
    updatePort(false);
  }
  else {
    int p = port;
    for (int i = 0; i < 5; i++) {
      portStr[4 - i] = p % 10;
      p /= 10;
    }
  }
}

void scene::Netplay::parseLine(Parser &parser) {
  if (parser.is("BGM", 2)) { // Override superclass
    bgm.createFromFile("", getResource(parser.getArg(1), Parser::EXT_MUSIC));
    bgmWait.createFromFile("",
                           getResource(parser.getArg(2), Parser::EXT_MUSIC));
  }
  else if (parser.is("SFX_CONNECT", 2)) {
    sndConStart = getResourceT<audio::Sound>(parser.getArg(1));
    sndConSuccess = getResourceT<audio::Sound>(parser.getArg(2));
  }
  else if (parser.is("LOGO", 1)) {
    imgLogo.createFromFile(getResource(parser.getArg(1), Parser::EXT_IMAGE));
  }
  else if (parser.is("SCANLINES", 2)) {
    imgScanlines.createFromFile(
        getResource(parser.getArg(1), Parser::EXT_IMAGE));
    imgStatic.createFromFile(getResource(parser.getArg(2), Parser::EXT_IMAGE));
  }
  else if (parser.is("MENU", 2)) {
    menuFont = getResourceT<Font>(parser.getArg(1));
    imgCursor.createFromFile(getResource(parser.getArg(2), Parser::EXT_IMAGE));
  }
  else if (parser.is("SFX_TV", 2)) {
    // Sounds
    sndOn = getResourceT<audio::Sound>(parser.getArg(1));
    sndOff = getResourceT<audio::Sound>(parser.getArg(2));
  }
  else {
    Scene::parseLine(parser);
  }
}

void scene::Netplay::parseJSON(const nlohmann::ordered_json &j_obj) {
  if (j_obj.contains("bgm")) { // Override superclass
    bgm.createFromFile("",
                       getResource(j_obj["bgm"].at("idle"), Parser::EXT_MUSIC));
    bgmWait.createFromFile(
        "", getResource(j_obj["bgm"].at("wait"), Parser::EXT_MUSIC));
  }
  if (j_obj.contains("sfx_connect")) {
    sndConStart = getResourceT<audio::Sound>(j_obj["sfx_connect"].at("start"));
    sndConSuccess =
        getResourceT<audio::Sound>(j_obj["sfx_connect"].at("success"));
  }
  if (j_obj.contains("logo")) {
    imgLogo.createFromFile(getResource(j_obj["logo"], Parser::EXT_IMAGE));
  }
  if (j_obj.contains("scanlines")) {
    imgScanlines.createFromFile(getResource(
        j_obj["scanlines"].at("image").at("scanlines"), Parser::EXT_IMAGE));
    imgStatic.createFromFile(getResource(
        j_obj["scanlines"].at("image").at("static"), Parser::EXT_IMAGE));
  }
  if (j_obj.contains("menu")) {
    menuFont = getResourceT<Font>(j_obj["menu"].at("font"));
    imgCursor.createFromFile(
        getResource(j_obj["menu"].at("cursorImage"), Parser::EXT_IMAGE));
  }
  if (j_obj.contains("sfx_tv")) {
    sndOn = getResourceT<audio::Sound>(j_obj["sfx_tv"].at("on"));
    sndOff = getResourceT<audio::Sound>(j_obj["sfx_tv"].at("off"));
  }

  // Do not call superclass version because "bgm" key is parsed differently
  // So copy others
  if (j_obj.contains("images")) {
    for (const auto &image : j_obj["images"]) {
      // Add a new image
      Image imgData;
      imgData.createFromFile(getResource(image["image"], Parser::EXT_IMAGE));
      if (imgData.exists()) {
        float x = image.at("pos").at("x");
        float y = image.at("pos").at("y");
        std::string szRender = image.value("renderType", "normal");
        Image::Render render = Image::Render::NORMAL;
        if (!szRender.compare("additive")) {
          render = Image::Render::ADDITIVE;
        }
        else if (!szRender.compare("subtractive")) {
          render = Image::Render::SUBTRACTIVE;
        }
        else if (!szRender.compare("multiply")) {
          render = Image::Render::MULTIPLY;
        }
        float xvel = 0.0f;
        float yvel = 0.0f;
        if (image.contains("vel")) {
          xvel = image["vel"].value("x", 0.0f);
          yvel = image["vel"].value("y", 0.0f);
        }
        bool wrap = image.value("wrap", false);
        images.emplace_back(imgData, x, y, 1.0f, render, xvel, yvel, wrap, 0);
      }
    }
  }
  if (j_obj.contains("sound")) {
    sndMenu = getResourceT<audio::Sound>(j_obj["sound"].at("menu"));
    sndSelect = getResourceT<audio::Sound>(j_obj["sound"].at("select"));
    sndBack = getResourceT<audio::Sound>(j_obj["sound"].at("back"));
    sndInvalid = getResourceT<audio::Sound>(j_obj["sound"].at("invalid"));
  }
}

#endif
