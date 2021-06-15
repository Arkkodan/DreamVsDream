#ifndef DVD_SHADER_H
#define DVD_SHADER_H

/// @brief RAII wrapper for OpenGL shaders
class Shader {
public:
  Shader();
  ~Shader();

  void setInt(const char *sz_variable, int value) const;
  void setFloat(const char *sz_variable, float value) const;
  void setVec2(const char *sz_variable, float value1, float value2) const;
  void setVec3(const char *sz_variable, float value1, float value2,
               float value3) const;
  void setVec4(const char *sz_variable, float value1, float value2,
               float value3, float value4) const;

  void use() const;

  bool create(const char *sz_vertex, const char *sz_fragment);

private:
  unsigned int program;
  unsigned int vertex;
  unsigned int fragment;
};

#endif // DVD_SHADER_H
