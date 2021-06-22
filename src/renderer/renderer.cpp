#include "renderer.h"

#include "gl_loader.h"

bool renderer::init() {
  // REQUIRE VERSION 1.3
  GLint versionMajor, versionMinor;
  glGetIntegerv(GL_MAJOR_VERSION, &versionMajor);
  glGetIntegerv(GL_MINOR_VERSION, &versionMinor);
  if (versionMajor == 1 && versionMinor < 3) {
    return false;
  }

  glEnable(GL_TEXTURE_2D); // Deprecated in OpenGL 3, will eventually remove
                           // once all texture renders are moved into shaders
  glDisable(GL_DEPTH_TEST);
  // glDisable(GL_LIGHTING); // Deprecated in OpenGL 3
  // glEnable(GL_CULL_FACE); // Disabled to allow winding-agnostic rendering
  // since we are only dealing in 2D

  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

#ifdef _WIN32
  if (!glActiveTexture || !glBlendEquation) {
    return false;
  }
#endif

  // Remove garbage data
  glClear(GL_COLOR_BUFFER_BIT);

  return true;
}
