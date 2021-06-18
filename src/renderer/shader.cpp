#include "shader.h"

renderer::Shader::Shader(GLenum type, const std::string &source) {
  shader = glCreateShader(type);

  const GLchar *const string = source.c_str();
  glShaderSource(shader, 1, &string, nullptr);
  glCompileShader(shader);

  GLint compileStatus;
  glGetShaderiv(shader, GL_COMPILE_STATUS, &compileStatus);
  if (compileStatus == GL_FALSE) {
    // TODO: Error handling
  }
}

renderer::Shader::~Shader() { glDeleteShader(shader); }

GLuint renderer::Shader::getShader() const { return shader; }

renderer::ShaderProgram::ShaderProgram(const Shader &vertexShader,
                                       const Shader &fragmentShader) {
  program = glCreateProgram();

  glAttachShader(program, vertexShader.getShader());
  glAttachShader(program, fragmentShader.getShader());

  glLinkProgram(program);

  GLint linkStatus;
  glGetProgramiv(program, GL_LINK_STATUS, &linkStatus);
  if (linkStatus == GL_FALSE) {
    // TODO: Error handling
  }

  uniformCache.clear();
}

renderer::ShaderProgram::~ShaderProgram() { glDeleteProgram(program); }

void renderer::ShaderProgram::use() const { glUseProgram(program); }

void renderer::ShaderProgram::unuse() { glUseProgram(0); }

void renderer::ShaderProgram::setVertexAttribPointer(
    GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride,
    const void *pointer) const {
  glEnableVertexAttribArray(index);
  glVertexAttribPointer(index, size, type, normalized, stride, pointer);
}

GLint renderer::ShaderProgram::getUniformLocation(
    const std::string &uniform) const {
  if (uniformCache.find(uniform) == uniformCache.cend()) {
    GLint uniformLocation = glGetUniformLocation(program, uniform.c_str());
    if (uniformLocation == -1) {
      // TODO: Error handling
    }
    uniformCache[uniform] = uniformLocation;
  }

  return uniformCache[uniform];
}

#define DEFINE_SET_UNIFORM(COUNT, TYPE_S, TYPE)                             \
  void renderer::ShaderProgram::setUniform##COUNT##TYPE_S##v(               \
      const std::string &uniform, const TYPE *value) const {                \
    glUniform##COUNT##TYPE_S##v(getUniformLocation(uniform), 1, &value[0]); \
  }

DEFINE_SET_UNIFORM(1, i, int);
DEFINE_SET_UNIFORM(1, f, float);
DEFINE_SET_UNIFORM(2, f, float);
DEFINE_SET_UNIFORM(3, f, float);
DEFINE_SET_UNIFORM(4, f, float);

#undef DEFINE_SET_UNIFORM

#define DEFINE_SET_UNIFORM_MATRIX(DIMENSION, TYPE_S, TYPE)                \
  void renderer::ShaderProgram::setUniformMatrix##DIMENSION##TYPE_S##v(   \
      const std::string &uniform, const TYPE *value) const {              \
    glUniformMatrix##DIMENSION##TYPE_S##v(getUniformLocation(uniform), 1, \
                                          false, &value[0]);              \
  }

DEFINE_SET_UNIFORM_MATRIX(4, f, float);

#undef DEFINE_SET_UNIFORM_MATRIX
