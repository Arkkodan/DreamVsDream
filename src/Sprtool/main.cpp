#include <iostream>
#include <stdio.h>

#ifndef _WIN32
#include <unistd.h>
#else
#ifndef WINVER
#define WINVER 0x0500
#endif
#include <shlwapi.h>
#include <windows.h>
#endif

#include "../DvD/error.h"
#include "../DvD/fighter.h"
#include "../DvD/graphics.h"
#include "../DvD/input.h"
#include "../DvD/parser.h"
#include "../DvD/shader_renderer/primitive_renderer.h"
#include "../DvD/sys.h"
#include "../util/fileIO.h"

game::Fighter fighter;
int frame = 0;
int anim = 0;

namespace init {
  extern void init();
}

void moveFile(std::string old, std::string nw) {
#ifdef _WIN32
  WCHAR *old16 = util::getPathUtf16(old);
  WCHAR *nw16 = util::getPathUtf16(nw);
  DWORD dwAttrib = GetFileAttributesW(nw16);
  if (dwAttrib != INVALID_FILE_ATTRIBUTES &&
      !(dwAttrib & FILE_ATTRIBUTE_DIRECTORY)) {
    DeleteFileW(nw16);
  }
  bool err = !MoveFileW(old16, nw16);
  free(nw16);
  free(old16);
#else
  unlink(nw.c_str());
  bool err = rename(old.c_str(), nw.c_str()) == -1;
#endif
  if (err) {
    error::error("Could not move file \"" + old + "\" to \"" + nw + "\"");
  }
}

namespace game {
  void Fighter::create(std::string name_) {
    name = name_;

    // Palettes
    palettes.clear();

    // Sprites
    Parser parser(util::getPath("chars/" + name + "/sprites.ubu"));

    // First pass
    nSprites = 0;
    while (parser.parseLine()) {
      if (parser.isGroup()) {
        nSprites++;
      }
    }
    sprites.resize(nSprites);

    // Parse the damned sprites for god's sake
    // Second pass
    parser.reset();

    int hitboxCounter = 0;
    int attackCounter = 0;
    int i = -1;
    while (parser.parseLine()) {
      int argc = parser.getArgC();
      if (parser.isGroup()) {
        // There's a new sprite definition! Load that shit
        i++;
        hitboxCounter = 0;
        attackCounter = 0;

        sprites[i].setName(parser.getArg(0));
        sprite::HitBoxGroup &hurtBoxes = sprites[i].getrDHurtBoxes();
        sprite::HitBoxGroup &hitBoxes = sprites[i].getrAHitBoxes();
        if (argc == 5) {
          sprites[i].setX(parser.getArgInt(1));
          sprites[i].setY(parser.getArgInt(2));

          hurtBoxes.init(parser.getArgInt(3));
          hitBoxes.init(parser.getArgInt(4));
        }
        else {
          sprites[i].setX(0);
          sprites[i].setY(0);

          hurtBoxes.init(0);
          hitBoxes.init(0);
        }

        // The image. Load it.
        sprites[i].getImage()->createFromFile("chars/" + name + "/sprites/" +
                                              parser.getArg(0) + ".png");
        continue;
      }

      if (argc == 4) {
        sprite::HitBoxGroup &hurtBoxes = sprites[i].getrDHurtBoxes();
        sprite::HitBoxGroup &hitBoxes = sprites[i].getrAHitBoxes();
        if (hitboxCounter < hurtBoxes.size) {
          hurtBoxes.boxes[hitboxCounter].pos.x = parser.getArgInt(0);
          hurtBoxes.boxes[hitboxCounter].pos.y = parser.getArgInt(1);
          hurtBoxes.boxes[hitboxCounter].size.x = parser.getArgInt(2);
          hurtBoxes.boxes[hitboxCounter].size.y = parser.getArgInt(3);
          hitboxCounter++;
        }
        else if (attackCounter < hitBoxes.size) {
          hitBoxes.boxes[attackCounter].pos.x = parser.getArgInt(0);
          hitBoxes.boxes[attackCounter].pos.y = parser.getArgInt(1);
          hitBoxes.boxes[attackCounter].size.x = parser.getArgInt(2);
          hitBoxes.boxes[attackCounter].size.y = parser.getArgInt(3);
          attackCounter++;
        }
      }
    }
  }

  void Fighter::saveSpr() {
    // Initialize buffers
    moveFile("chars/" + name + "/sprites.ubu",
             "chars/" + name + "/sprites.ubu.bak");
    FILE *out = util::ufopen("chars/" + name + "/sprites.ubu", "wb");
    if (!out) {
      error::error("Could not write to file \"chars/" + name +
                   "/sprites.ubu\"");
      return;
    }

    for (int i = 0; i < nSprites; i++) {
      const sprite::HitBoxGroup &hurtBoxes = sprites[i].getrDHurtBoxes();
      const sprite::HitBoxGroup &hitBoxes = sprites[i].getrAHitBoxes();
      fprintf(out, "\r\n[%s %d, %d, %d, %d]\r\n", sprites[i].getName().c_str(),
              sprites[i].getX(), sprites[i].getY(), hurtBoxes.size,
              hitBoxes.size);
      for (int j = 0; j < hurtBoxes.size; j++) {
        fprintf(out, "%d, %d, %d, %d\r\n", hurtBoxes.boxes[j].pos.x,
                hurtBoxes.boxes[j].pos.y, hurtBoxes.boxes[j].size.x,
                hurtBoxes.boxes[j].size.y);
      }
      for (int j = 0; j < hitBoxes.size; j++) {
        fprintf(out, "%d, %d, %d, %d\r\n", hitBoxes.boxes[j].pos.x,
                hitBoxes.boxes[j].pos.y, hitBoxes.boxes[j].size.x,
                hitBoxes.boxes[j].size.y);
      }
    }
    fclose(out);
  }
} // namespace game

#ifdef _WIN32
int main(int foo, char **bar)
#else
int main(int argc, char **argv)
#endif
{
#ifdef _WIN32
  // Get argc/argv
  int argc = 0;
  WCHAR **argv = CommandLineToArgvW(GetCommandLineW(), &argc);
  if (argc < 2) {
    LocalFree(argv);
    return 1;
  }
  char *name8 = util::utf16to8(argv[1]);
  LocalFree(argv);
  std::string name = name8;
  free(name8);
#else
  if (argc < 2) {
    std::cerr << "error: no fighter specified." << std::endl;
    return 1;
  }
  std::string name = argv[1];
#endif

  init::init();

  fighter.create(name);

  for (;;) {
    sys::refresh();

    // Draw a crosshair
    if (input::isBlackBG()) {
      renderer::PrimitiveRenderer::setColor(1.0f, 1.0f, 1.0f, 1.0f);
    }
    else {
      renderer::PrimitiveRenderer::setColor(0.0f, 0.0f, 0.0f, 1.0f);
    }
    renderer::PrimitiveRenderer::setPosRect(
        sys::WINDOW_WIDTH / 2 - 100, sys::WINDOW_WIDTH / 2 + 99,
        sys::FLIP(sys::EDIT_OFFSET) + 1, sys::FLIP(sys::EDIT_OFFSET));
    renderer::PrimitiveRenderer::draw();
    renderer::PrimitiveRenderer::setPosRect(
        sys::WINDOW_WIDTH / 2 - 1, sys::WINDOW_WIDTH / 2 + 1,
        sys::FLIP(sys::EDIT_OFFSET) + 5, sys::FLIP(sys::EDIT_OFFSET) - 4);
    renderer::PrimitiveRenderer::draw();
    renderer::ShaderProgram::unuse();

    fighter.getSpriteAt(frame)->draw(0, sys::EDIT_OFFSET, false, 1.0f);
  }

  return 0;
}
