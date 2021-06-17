#include "select.h"

#include "fight.h"
#include "scene.h"
#include "versus.h"

#include "../graphics.h"
#include "../network.h"
#include "../resource_manager.h"
#include "../shader_renderer/primitive_renderer.h"
#include "../shader_renderer/texture2D_renderer.h"
#include "../stage.h"
#include "../sys.h"

#include <algorithm>

namespace {
  static constexpr auto STAGES_PER_ROW = 10;
  static constexpr auto STAGE_ICON_WIDTH = 76;
} // namespace

scene::Select::Select() : Scene("select") {
  width = height = 0;
  gWidth = gHeight = 0;
  gridC = 0;

  cursor_stage = 0;
  cursor_stage_offset = 0;
  font_stage = nullptr;
}

scene::Select::~Select() {}

void scene::Select::init() {
  resource_manager::loadFromManifest<game::Fighter>();
  fighters = resource_manager::getFromManifest<game::Fighter>();

  Scene::init();
}

void scene::Select::think() {
  Scene::think();

  // Visual effects animations

  for (int i = 0; i < 2; i++) {
    if (cursors[i].timerPortrait) {
      if (cursors[i].timerPortrait == 1) {
        cursors[i].timerPortrait = 0;
      }
      else {
        cursors[i].timerPortrait *= 0.8;
      }
    }
  }

  // Move cursor
  if (cursors[0].lockState != Cursor::CURSOR_LOCKED ||
      cursors[1].lockState != Cursor::CURSOR_LOCKED) {
    for (int cur = 0; cur < 2; cur++) {
      uint16_t input = Fight::madotsuki.frameInput;
      if (cur == 1) {
        input = Fight::poniko.frameInput;
      }
      if (FIGHT->gametype == Fight::GAMETYPE_TRAINING) {
        if (cursors[0].lockState == Cursor::CURSOR_LOCKED) {
          cur = 1;
        }
        else {
          cur = 0;
        }
      }
      int group = cursors[cur].getGroup(width, gWidth, gHeight);

      if (cursors[cur].lockState == Cursor::CURSOR_UNLOCKED) {
        if (input & game::INPUT_DIRMASK) {
          // Old cursor pos SET
          cursors[cur].posOld = cursors[cur].pos;
          cursors[cur].timerPortrait = PORTRAIT_FADE;
        }

        if (input & game::INPUT_LEFT) {
          sndMenu->play();

          do {
            if (cursors[cur].pos % width == 0) {
              cursors[cur].pos += width - 1;
            }
            else {
              cursors[cur].pos--;
            }
          } while (grid[cursors[cur].pos].x == -1);

          // Check cell limits
          if (cursors[cur].pos >= gridC) {
            cursors[cur].pos += gridC % width - (cursors[cur].pos % width) - 1;
          }
        }
        else if (input & game::INPUT_RIGHT) {
          sndMenu->play();

          do {
            if (cursors[cur].pos % width == width - 1) {
              cursors[cur].pos -= width - 1;
            }
            else {
              cursors[cur].pos++;
            }
          } while (grid[cursors[cur].pos].x == -1);

          // Check cell limits
          if (cursors[cur].pos >= gridC) {
            cursors[cur].pos -= cursors[cur].pos % width;
          }
        }
        else if (input & game::INPUT_UP) {
          sndMenu->play();

          do {
            if (cursors[cur].pos / width == 0) {
              cursors[cur].pos += width * (height - 1);
            }
            else {
              cursors[cur].pos -= width;
            }
          } while (grid[cursors[cur].pos].x == -1);

          // Check cell limits
          if (cursors[cur].pos >= gridC) {
            if (cursors[cur].pos % width < gridC % width) {
              cursors[cur].pos -= width * (gridC / width - 1);
            }
            else {
              cursors[cur].pos -= width * (gridC / width);
            }
          }
        }
        else if (input & game::INPUT_DOWN) {
          sndMenu->play();

          do {
            if (cursors[cur].pos / width == height - 1) {
              cursors[cur].pos -= width * (height - 1);
            }
            else {
              cursors[cur].pos += width;
            }
          } while (grid[cursors[cur].pos].x == -1);

          // Check cell limits
          if (cursors[cur].pos >= gridC) {
            cursors[cur].pos = cursors[cur].pos % width;
          }
        }

        if (input & game::INPUT_A) {
          if (gridFighters[cursors[cur].pos] >= 0) {
            if (cur == 0) {
              Fight::madotsuki.palette = 0;
            }
            else {
              Fight::poniko.palette = 0;
            }
            cursors[cur].lockState = Cursor::CURSOR_COLORSWAP;
            newEffect(cur, group);
          }
          else {
            sndInvalid->play();
          }
        }
      }
      else if (cursors[cur].lockState == Cursor::CURSOR_COLORSWAP) {
        int nPalettes =
            fighters[gridFighters[cursors[cur].pos]]->getPaletteCount();
        if (input & game::INPUT_LEFT) {
          sndMenu->play();

          if (cur == 0) {
            if (Fight::madotsuki.palette == 0) {
              Fight::madotsuki.palette = nPalettes - 1;
            }
            else {
              Fight::madotsuki.palette--;
            }
          }
          else {
            if (Fight::poniko.palette == 0) {
              Fight::poniko.palette = nPalettes - 1;
            }
            else {
              Fight::poniko.palette--;
            }
          }
        }
        else if (input & game::INPUT_RIGHT) {
          sndMenu->play();

          if (cur == 0) {
            if (Fight::madotsuki.palette == nPalettes - 1) {
              Fight::madotsuki.palette = 0;
            }
            else {
              Fight::madotsuki.palette++;
            }
          }
          else {
            if (Fight::poniko.palette == nPalettes - 1) {
              Fight::poniko.palette = 0;
            }
            else {
              Fight::poniko.palette++;
            }
          }
        }

        if (input & game::INPUT_A) {
          sndSelect->play();
          cursors[cur].lockState = Cursor::CURSOR_LOCKED;
        }
      }

      if (input & game::INPUT_B) {
        if (FIGHT->gametype == scene::Fight::GAMETYPE_TRAINING) {
          if (cursors[0].lockState == Cursor::CURSOR_UNLOCKED) {
            sndBack->play();
            setScene(SCENE_TITLE);
          }
          else if (cursors[cur].lockState == Cursor::CURSOR_UNLOCKED) {
            sndBack->play();
            cur--;
            cursors[cur].lockState = Cursor::CURSOR_COLORSWAP;
          }
          else if (cursors[cur].lockState == Cursor::CURSOR_COLORSWAP) {
            cursors[cur].lockState = Cursor::CURSOR_UNLOCKED;

            if (!curData.empty()) {
              group = cursors[cur].getGroup(width, gWidth, gHeight);
              curData[group].sndDeselect->play();
              curData[group].sndSelect->stop();
            }
          }
        }
        else {
          if (cursors[cur].lockState == Cursor::CURSOR_LOCKED) {
            sndBack->play();
            cursors[cur].lockState = Cursor::CURSOR_COLORSWAP;
          }
          else if (cursors[cur].lockState == Cursor::CURSOR_COLORSWAP) {
            cursors[cur].lockState = Cursor::CURSOR_UNLOCKED;
            if (!curData.empty()) {
              group = cursors[cur].getGroup(width, gWidth, gHeight);
              curData[group].sndDeselect->play();
              curData[group].sndSelect->stop();
            }
          }
          else {
            if (!net::isConnected()) {
              sndBack->play();
              setScene(SCENE_TITLE);
            }
          }
        }
      }

      if (FIGHT->gametype == scene::Fight::GAMETYPE_TRAINING) {
        break;
      }
    }
  }
  else {
    cursor_stage_offset *= 0.36f; // 0.95f ^ 20
    if (std::abs(cursor_stage_offset) < 1) {
      cursor_stage_offset = 0.0f;
    }

    const auto &stages = Stage::getcrStages();
    int size = stages.size();
    int exists =
        std::count_if(stages.cbegin(), stages.cend(),
                      [](const Stage *stage) { return stage->isExists(); });
    if (size > 0 && exists > 0) {
      int dx = (input(game::INPUT_LEFT) ? -1 : 0) +
               (input(game::INPUT_RIGHT) ? 1 : 0);
      int dy =
          (input(game::INPUT_UP) ? -1 : 0) + (input(game::INPUT_DOWN) ? 1 : 0);

      switch (dx) {
      case -1: // Left
        sndMenu->play();
        do {
          if (cursor_stage % STAGES_PER_ROW == 0) {
            // Wrap-around
            cursor_stage += STAGES_PER_ROW - 1;
            cursor_stage_offset += STAGE_ICON_WIDTH * (STAGES_PER_ROW - 1);
          }
          else {
            cursor_stage--;
            cursor_stage_offset -= STAGE_ICON_WIDTH;
          }
        } while (cursor_stage >= size || !stages[cursor_stage]->isExists());
        break;
      case 1: // Right
        sndMenu->play();
        do {
          if (cursor_stage % STAGES_PER_ROW == STAGES_PER_ROW - 1) {
            // Wrap-around
            cursor_stage -= STAGES_PER_ROW - 1;
            cursor_stage_offset -= STAGE_ICON_WIDTH * (STAGES_PER_ROW - 1);
          }
          else {
            cursor_stage++;
            cursor_stage_offset += STAGE_ICON_WIDTH;
          }
        } while (cursor_stage >= size || !stages[cursor_stage]->isExists());
        break;
      }

      switch (dy) {
      case -1: // Up
        sndMenu->play();
        do {
          if (cursor_stage - STAGES_PER_ROW < 0) {
            // Wrap-around, multiple of STAGES_PER_ROW
            cursor_stage += size / STAGES_PER_ROW * STAGES_PER_ROW;
            if (cursor_stage >= size) {
              // Adjust into size if needed
              cursor_stage -= STAGES_PER_ROW;
            }
          }
          else {
            cursor_stage -= STAGES_PER_ROW;
          }
        } while (cursor_stage >= size || !stages[cursor_stage]->isExists());
        break;
      case 1: // Down
        sndMenu->play();
        do {
          cursor_stage += STAGES_PER_ROW;
          if (cursor_stage >= size) {
            // Wrap-around, into first row
            cursor_stage %= STAGES_PER_ROW;
          }
        } while (cursor_stage >= size || !stages[cursor_stage]->isExists());
        break;
      }
    }

    if (FIGHT->gametype == Fight::GAMETYPE_TRAINING) {
      if (input(game::INPUT_B)) {
        cursors[1].lockState = Cursor::CURSOR_COLORSWAP;

        if (!curData.empty()) {
          int group = cursors[1].getGroup(width, gWidth, gHeight);
          curData[group].sndDeselect->play();
          curData[group].sndSelect->stop();
        }
      }
    }
    else {
      if (input(game::INPUT_B)) {
        sndBack->play();
        cursors[0].lockState = Cursor::CURSOR_COLORSWAP;
        cursors[1].lockState = Cursor::CURSOR_COLORSWAP;

        /*if(curData) {
                int group = cursors[0].getGroup(width, gWidth, gHeight);
                curData[group].sndDeselect.play();
                curData[group].sndSelect.stop();
                group = cursors[1].getGroup(width, gWidth, gHeight);
                curData[group].sndDeselect.play();
                curData[group].sndSelect.stop();
        }*/
      }
    }

    if (input(game::INPUT_A)) {
      if (cursor_stage < size && stages[cursor_stage]->isExists()) {
        // Start game!
        Fight::madotsuki.fighter = fighters[gridFighters[cursors[0].pos]];
        Fight::poniko.fighter = fighters[gridFighters[cursors[1].pos]];

        Versus *versus =
            reinterpret_cast<Versus *>(getSceneFromIndex(SCENE_VERSUS));
        versus->portraits[0] = Fight::madotsuki.fighter->getcImagePortrait();
        versus->portraits[1] = Fight::poniko.fighter->getcImagePortrait();

        Stage::setStageIndex(cursor_stage);
        setScene(SCENE_VERSUS);
        sndSelect->play();
      }
      else {
        sndInvalid->play();
      }
    }
  }
}

void scene::Select::reset() {
  Scene::reset();

  for (int i = 0; i < 2; i++) {
    cursors[i].lockState = Cursor::CURSOR_UNLOCKED;
    cursors[i].timer = 0;
    cursors[i].timerPortrait = 0;
    cursors[i].frame = 0;
  }
  cursors[0].pos = cursors[0].posDefault;
  cursors[1].pos = cursors[1].posDefault;

  cursor_stage = 0;
  // Find the first stage that exists
  for (const auto &stage : Stage::getcrStages()) {
    if (stage->isExists()) {
      break;
    }
    cursor_stage++;
  }
}

void scene::Select::draw() const {
  Scene::draw();

  // Draw portraits first
  if (cursors[0].lockState == Cursor::CURSOR_LOCKED ||
      FIGHT->gametype == Fight::GAMETYPE_VERSUS) {
    if (cursors[1].timerPortrait) {
      if (gridFighters[cursors[1].posOld] >= 0) {
        const Image *portrait =
            fighters[gridFighters[cursors[1].posOld]]->getcImagePortrait();
        renderer::Texture2DRenderer::setColor(
            1.0f, 1.0f, 1.0f,
            static_cast<float>(cursors[1].timerPortrait) / PORTRAIT_FADE);
        portrait->draw<renderer::Texture2DRenderer>(
            sys::WINDOW_WIDTH - portrait->getW() +
                (PORTRAIT_FADE - cursors[1].timerPortrait),
            0, true);
      }
    }
    if (gridFighters[cursors[1].pos] >= 0) {
      const Image *portrait =
          fighters[gridFighters[cursors[1].pos]]->getcImagePortrait();
      renderer::Texture2DRenderer::setColor(
          1.0f, 1.0f, 1.0f,
          static_cast<float>(PORTRAIT_FADE - cursors[1].timerPortrait) /
              PORTRAIT_FADE);
      portrait->draw<renderer::Texture2DRenderer>(
          sys::WINDOW_WIDTH - portrait->getW() + cursors[1].timerPortrait, 0,
          true);
    }
  }
  if (cursors[0].timerPortrait) {
    if (gridFighters[cursors[0].posOld] >= 0) {
      const Image *portrait =
          fighters[gridFighters[cursors[0].posOld]]->getcImagePortrait();
      renderer::Texture2DRenderer::setColor(
          1.0f, 1.0f, 1.0f,
          static_cast<float>(cursors[0].timerPortrait) / PORTRAIT_FADE);
      portrait->draw<renderer::Texture2DRenderer>(
          0 - (PORTRAIT_FADE - cursors[0].timerPortrait), 0);
    }
  }
  if (gridFighters[cursors[0].pos] >= 0) {
    const Image *portrait =
        fighters[gridFighters[cursors[0].pos]]->getcImagePortrait();
    renderer::Texture2DRenderer::setColor(
        1.0f, 1.0f, 1.0f,
        static_cast<float>(PORTRAIT_FADE - cursors[0].timerPortrait) /
            PORTRAIT_FADE);
    portrait->draw<renderer::Texture2DRenderer>(0 - cursors[0].timerPortrait,
                                                0);
  }

  // Now the GUI
  std::for_each(gui.cbegin(), gui.cend(),
                [](const SceneImage &si) { si.draw(false); });

  if (cursors[1].lockState >= Cursor::CURSOR_COLORSWAP) {
    if (gridFighters[cursors[1].pos] >= 0) {
      game::Fighter *fighter = fighters[gridFighters[cursors[1].pos]];
      const sprite::Sprite *spr = fighter->getcSpriteAt(0);
      int atlas_sprite = spr->getAtlasSprite();
      const Atlas *atlas = spr->getcAtlas();
      AtlasSprite sprAtlas = atlas->getSprite(atlas_sprite);

      graphics::setPalette(fighter->getcrPalettes()[Fight::poniko.palette],
                           1.0f, 1.0f, 1.0f, 1.0f, 0.0f);
      atlas->draw(atlas_sprite,
                  sys::WINDOW_WIDTH - 50 + spr->getX() - sprAtlas.w,
                  sys::WINDOW_HEIGHT - 40 - spr->getY() - sprAtlas.h, true);
      renderer::ShaderProgram::unuse();
    }
  }
  if (cursors[0].lockState >= Cursor::CURSOR_COLORSWAP) {
    if (gridFighters[cursors[0].pos] >= 0) {
      game::Fighter *fighter = fighters[gridFighters[cursors[0].pos]];
      const sprite::Sprite *spr = fighter->getcSpriteAt(0);
      int atlas_sprite = spr->getAtlasSprite();
      const Atlas *atlas = spr->getcAtlas();
      AtlasSprite sprAtlas = atlas->getSprite(atlas_sprite);

      graphics::setPalette(fighter->getcrPalettes()[Fight::madotsuki.palette],
                           1.0f, 1.0f, 1.0f, 1.0f, 0.0f);
      atlas->draw(atlas_sprite, 50 - spr->getX(),
                  sys::WINDOW_HEIGHT - 40 - spr->getY() - sprAtlas.h, false);
      renderer::ShaderProgram::unuse();
    }
  }

  // Draw the select sprites
  for (int i = 0; i < gridC; i++) {
    if (gridFighters[i] >= 0) {
      fighters[gridFighters[i]]
          ->getcImageSelect()
          ->draw<renderer::Texture2DRenderer>(grid[i].x, grid[i].y);
    }
  }

  // Cursor
  // Get the current group
  if (!curData.empty()) {
    int count = 1;
    if (FIGHT->gametype == Fight::GAMETYPE_TRAINING &&
        cursors[0].lockState != Cursor::CURSOR_LOCKED) {
      count = 0;
    }
    for (int i = 0; i <= count; i++) {
      int group = cursors[i].getGroup(width, gWidth, gHeight);
      ;

      if (cursors[i].lockState == Cursor::CURSOR_UNLOCKED) {
        renderer::Texture2DRenderer::setColor(cursors[i].r / 255.0f,
                                              cursors[i].g / 255.0f,
                                              cursors[i].b / 255.0f, 1.0f);
      }
      curData[group].img.draw<renderer::Texture2DRenderer>(
          grid[cursors[i].pos].x + curData[group].off.x,
          grid[cursors[i].pos].y + curData[group].off.y);

      // Draw the effects
      drawEffect(i, group, grid[cursors[i].pos].x, grid[cursors[i].pos].y);
    }
  }

  // Finally draw the stage selection screen
  if (cursors[0].lockState == Cursor::CURSOR_LOCKED &&
      cursors[1].lockState == Cursor::CURSOR_LOCKED) {
    // Darken background
    renderer::PrimitiveRenderer::setColor(0.0f, 0.0f, 0.0f, 0.7f);
    renderer::PrimitiveRenderer::setPosRect(0.0f, sys::WINDOW_WIDTH,
                                            sys::WINDOW_HEIGHT, 0.0f);
    renderer::PrimitiveRenderer::draw();
    renderer::ShaderProgram::unuse();

    // Draw the stage list
    constexpr auto STAGE_ICON_PADDING = 8;
    const auto &stages = Stage::getcrStages();
    for (int i = 0, size = stages.size(); i < size; i++) {
      int x = static_cast<int>(
          (STAGE_ICON_WIDTH + STAGE_ICON_PADDING) *
              (i % STAGES_PER_ROW + 3 - cursor_stage % STAGES_PER_ROW) +
          STAGE_ICON_WIDTH / 2 + cursor_stage_offset);
      int y = 150 + (STAGE_ICON_PADDING + 50) * (i / STAGES_PER_ROW);
      auto *stage = stages[i];
      if (stage->isExists()) {
        Animation *thumbnail = stage->getThumbnail();
        if (!thumbnail->isPlaying())
          thumbnail->setPlaying(true);
        if (cursor_stage == i) {
          renderer::Texture2DRenderer::setColor(1.0f, 1.0f, 1.0f, 1.0f);
          thumbnail->draw(x, y);
        }
        else {
          renderer::Texture2DRenderer::setColor(0.5f, 0.5f, 0.5f, 1.0f);
          thumbnail->draw(x, y);
        }
      }
    }
  }
}

void scene::Select::newEffect(int player, int group) {
  if (!curData.empty()) {
    curData[group].sndSelect->play();
    curData[group].sndDeselect->stop();
  }

  cursors[player].frame = 1;
}

void scene::Select::drawEffect(int player, int group, int _x, int _y,
                               bool spr) const {
  if (cursors[player].frame) {
    float scale = 1.0f;
    float alpha =
        1.0f - (cursors[player].frame - 1) / (float)curData[group].frameC;
    ;
    if (curData[group].grow) {
      scale += (cursors[player].frame - 1) * 0.1f;
    }
    int x =
        (cursors[player].frame - 1) % (curData[group].imgSelect.getH() / 96);

    graphics::setRect(0, x * 96, 96, 96);
    renderer::Texture2DRenderer::setColor(1.0f, 1.0f, 1.0f, alpha);

    if (spr) {
      graphics::setScale(scale * 2);
      curData[group].imgSelect.drawSprite<renderer::Texture2DRenderer>(
          _x - (96 * scale), _y - (96 * scale));
    }
    else {
      graphics::setScale(scale);
      curData[group].imgSelect.draw<renderer::Texture2DRenderer>(
          _x - (96 / 2 * scale) + 26 / 2, _y - (96 / 2 * scale) + 29 / 2);
    }

    if (++cursors[player].timer > curData[group].speed) {
      cursors[player].timer = 0;
      if (++cursors[player].frame > curData[group].frameC) {
        cursors[player].frame = 0;
      }
    }
  }
}

void scene::Select::parseLine(Parser &parser) {
  int argc = parser.getArgC();
  if (parser.is("GRID", 4)) {
    // Make the grids!
    width = parser.getArgInt(1);
    height = parser.getArgInt(2);

    grid.resize(width * height);
    gridFighters.resize(width * height);

    // Now for the groups...
    gWidth = parser.getArgInt(3);
    gHeight = parser.getArgInt(4);

    int gSize = (width / gWidth) * (height / gHeight);
    curData.resize(gSize);

    // Set the p2 cursor to the width - 1
    cursors[1].pos = width - 1;
    cursors[1].posOld = width - 1;
  }
  else if (parser.is("CURSOR", 10)) {
    // Load up the cursor

    // Get the group num
    int group = parser.getArgInt(1) - 1;

    // Cursor Image
    curData[group].img.createFromFile(
        getResource(parser.getArg(2), Parser::EXT_IMAGE));

    // X & Y offsets
    curData[group].off.x = parser.getArgInt(3);
    curData[group].off.y = parser.getArgInt(4);

    // Effect stuff
    curData[group].imgSelect.createFromFile(
        getResource(parser.getArg(5), Parser::EXT_IMAGE));
    curData[group].frameC = parser.getArgInt(6);
    curData[group].speed = parser.getArgInt(7);
    curData[group].grow = parser.getArgBool(8, false);

    // Sounds
    curData[group].sndSelect = getResourceT<audio::Sound>(parser.getArg(9));
    curData[group].sndDeselect = getResourceT<audio::Sound>(parser.getArg(10));
  }
  else if (parser.is("CHAR", 1)) {
    // Add to the grids
    gridFighters[gridC] = -1;

    // Is it a null?
    if (!parser.getArg(1).compare("null")) {
      grid[gridC].x = -1;
      grid[gridC].y = -1;
      gridC++;
      return;
    }

    if (argc < 3) {
      return;
    }
    if (gridC >= width * height) {
      return;
    }

    // Get fighter
    for (int i = 0, fightersSize = fighters.size(); i < fightersSize; i++) {
      if (!fighters[i]->getDataName().compare(parser.getArg(1))) {
        gridFighters[gridC] = i;
        break;
      }
    }

    // Position
    grid[gridC].x = parser.getArgInt(2);
    grid[gridC].y = parser.getArgInt(3);

    gridC++;
  }
  else if (parser.is("SELECT", 2)) {
    float x = parser.getArgFloat(2);
    float y = parser.getArgFloat(3);
    Image::Render render = Image::Render::NORMAL;
    float xvel = 0.0f;
    float yvel = 0.0f;
    bool wrap = false;
    if (argc > 4) {
      std::string szRender = parser.getArg(4);
      if (!szRender.compare("additive")) {
        render = Image::Render::ADDITIVE;
      }
      else if (!szRender.compare("subtractive")) {
        render = Image::Render::SUBTRACTIVE;
      }
      else if (!szRender.compare("multiply")) {
        render = Image::Render::MULTIPLY;
      }
      if (argc > 5) {
        xvel = parser.getArgFloat(5);
        if (argc > 6) {
          yvel = parser.getArgFloat(6);
          if (argc > 7) {
            wrap = parser.getArgBool(7, false);
          }
        }
      }
    }

    // Add a new image
    Image imgData;
    imgData.createFromFile(getResource(parser.getArg(1), Parser::EXT_IMAGE));
    if (!imgData.exists()) {
      return;
    }
    gui.emplace_back(imgData, x, y, 1.0f, render, xvel, yvel, wrap, 0);
  }
  else if (parser.is("STAGES", 1)) {
    // Load the font
    font_stage = getResourceT<Font>(parser.getArg(1));
  }
  else if (parser.is("PLAYER", 6)) {
    // Load the player
    int p = parser.getArgInt(1) - 1;

    // Set the default position of the cursor
    cursors[p].posDefault = parser.getArgInt(3) * width + parser.getArgInt(2);
    cursors[p].pos = cursors[p].posDefault;

    // Colors
    cursors[p].r = parser.getArgInt(4);
    cursors[p].g = parser.getArgInt(5);
    cursors[p].b = parser.getArgInt(6);
  }
  else {
    Scene::parseLine(parser);
  }
}

void scene::Select::parseJSON(const nlohmann::ordered_json &j_obj) {
  if (j_obj.contains("grid")) {
    // Make the grids!
    width = j_obj["grid"].at("w");
    height = j_obj["grid"].at("h");

    grid.resize(width * height);
    gridFighters.resize(width * height);

    // Now for the groups...
    gWidth = j_obj["grid"].at("w_group");
    gHeight = j_obj["grid"].at("h_group");

    int gSize = (width / gWidth) * (height / gHeight);
    curData.resize(gSize);

    // Set the p2 cursor to the width - 1
    cursors[1].pos = width - 1;
    cursors[1].posOld = width - 1;
  }
  if (j_obj.contains("cursors")) {
    for (int i = 0, size = j_obj["cursors"].size(); i < size; i++) {
      const auto &cursor = j_obj["cursors"][i];
      curData[i].img.createFromFile(
          getResource(cursor.at("image"), Parser::EXT_IMAGE));

      curData[i].off.x = cursor.at("offset").at("x");
      curData[i].off.y = cursor.at("offset").at("y");

      curData[i].imgSelect.createFromFile(
          getResource(cursor.at("effect").at("image"), Parser::EXT_IMAGE));
      curData[i].frameC = cursor.at("effect").at("frameCount");
      curData[i].speed = cursor.at("effect").at("speed");
      curData[i].grow = cursor.at("effect").at("grow");

      curData[i].sndSelect =
          getResourceT<audio::Sound>(cursor.at("sound").at("select"));
      curData[i].sndDeselect =
          getResourceT<audio::Sound>(cursor.at("sound").at("deselect"));
    }
  }
  if (j_obj.contains("chars")) {
    gridC = std::min(static_cast<int>(j_obj["chars"].size()), width * height);
    for (int i = 0; i < gridC; i++) {
      const auto &charI = j_obj["chars"][i];
      gridFighters[i] = -1;

      if (charI.is_null()) {
        grid[i].x = -1;
        grid[i].y = -1;
        continue;
      }

      std::string name = charI.at("name");
      for (int j = 0, fightersSize = fighters.size(); j < fightersSize; j++) {
        if (!fighters[j]->getDataName().compare(name)) {
          gridFighters[i] = j;
          break;
        }
      }

      grid[i].x = charI.at("pos").at("x");
      grid[i].y = charI.at("pos").at("y");
    }
  }
  if (j_obj.contains("select")) {
    for (const auto &image : j_obj["select"]) {
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
        gui.emplace_back(imgData, x, y, 1.0f, render, xvel, yvel, wrap, 0);
      }
    }
  }
  if (j_obj.contains("stages")) {
    font_stage = getResourceT<Font>(j_obj["stages"].at("font"));
  }
  if (j_obj.contains("players")) {
    for (int i = 0, size = j_obj["players"].size(); i < size; i++) {
      const auto &player = j_obj["players"][i];
      cursors[i].posDefault = cursors[i].pos =
          static_cast<int>(player.at("posDefault").at("y")) * width +
          static_cast<int>(player.at("posDefault").at("x"));

      cursors[i].r = player.at("color").at("r");
      cursors[i].g = player.at("color").at("g");
      cursors[i].b = player.at("color").at("b");
    }
  }
  Scene::parseJSON(j_obj);
}

// CURSOR DATA
scene::CursorData::CursorData() {
  frameC = 0;
  speed = 0;
  grow = false;
  sndSelect = sndDeselect = nullptr;
}

scene::CursorData::~CursorData() {}

scene::CursorData::CursorData(CursorData &&other) noexcept
    : off(std::move(other.off)), img(std::move(other.img)),
      imgSelect(std::move(other.imgSelect)), sndSelect(other.sndSelect),
      sndDeselect(other.sndDeselect), frameC(other.frameC), speed(other.speed),
      grow(other.grow) {}

scene::CursorData &scene::CursorData::operator=(CursorData &other) noexcept {
  off = std::move(other.off);
  img = std::move(other.img);
  imgSelect = std::move(other.imgSelect);
  sndSelect = other.sndSelect;
  sndDeselect = other.sndDeselect;

  frameC = other.frameC;
  speed = other.speed;
  grow = other.grow;

  return *this;
}

// CURSOR
scene::Cursor::Cursor() {
  pos = posOld = posDefault = 0;
  frame = 0;
  timer = 0;

  timerPortrait = 0;

  lockState = CURSOR_UNLOCKED;

  r = g = b = 0;
}

int scene::Cursor::getGroup(int w, int gW, int gH) const {
  return (pos % w) / gW + ((pos / w) / gH) * (w / gW);
}
