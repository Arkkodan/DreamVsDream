#ifndef RENDERER_SHADER_H
#define RENDERER_SHADER_H

#include "gl_loader.h"

#include <string>
#include <unordered_map>

namespace renderer {
  class Shader {
  public:
    Shader(GLenum type, const std::string &source);
    ~Shader();

    GLuint getShader() const;

  private:
    GLuint shader;
  };

  class ShaderProgram {
  public:
    ShaderProgram(const Shader &vertexShader, const Shader &fragmentShader);
    ~ShaderProgram();

    void use() const;
    static void unuse();

    void setVertexAttribPointer(GLuint index, GLint size, GLenum type,
                                GLboolean normalized, GLsizei stride,
                                const void *pointer) const;

#define elementcountof(type, member) \
  sizeof(((type *)0)->member) / sizeof((((type *)0)->member)[0])

#define setVAP(index, vertex, attrib, type, normalized)                        \
  setVertexAttribPointer(                                                      \
      index, elementcountof(vertex, attrib), type, normalized, sizeof(vertex), \
      reinterpret_cast<const void *>(offsetof(vertex, attrib)))

    //#undef elementcountof

#define DECLARE_SET_UNIFORM(COUNT, TYPE_S, TYPE)                \
  void setUniform##COUNT##TYPE_S##v(const std::string &uniform, \
                                    const TYPE *value) const;
    DECLARE_SET_UNIFORM(1, i, int);
    DECLARE_SET_UNIFORM(1, f, float);
    DECLARE_SET_UNIFORM(2, f, float);
    DECLARE_SET_UNIFORM(3, f, float);
    DECLARE_SET_UNIFORM(4, f, float);
#undef DECLARE_SET_UNIFORM

#define DECLARE_SET_UNIFORM_MATRIX(DIMENSION, TYPE_S, TYPE)               \
  void setUniformMatrix##DIMENSION##TYPE_S##v(const std::string &uniform, \
                                              const TYPE *value) const;
    DECLARE_SET_UNIFORM_MATRIX(4, f, float);
#undef DECLARE_SET_UNIFORM_MATRIX

  private:
    GLint getUniformLocation(const std::string &uniform) const;

  private:
    GLuint program;

    mutable std::unordered_map<std::string, GLint> uniformCache;
  };
} // namespace renderer

#endif // RENDERER_SHADER_H
