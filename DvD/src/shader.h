#ifndef SHADER_H_INCLUDED
#define SHADER_H_INCLUDED

#include "globals.h"

class Shader
{
public:
    Shader();
    ~Shader();

    void setInt(const char* sz_variable, int value);
    void setFloat(const char* sz_variable, float value);
    void setVec2(const char* sz_variable, float value1, float value2);
    void setVec3(const char* sz_variable, float value1, float value2, float value3);
    void setVec4(const char* sz_variable, float value1, float value2, float value3, float value4);

    void use();

    bool create(const char* sz_vertex, const char* sz_fragment);

private:
    unsigned int shader_program;
    unsigned int shader_vertex;
    unsigned int shader_fragment;
};

#endif // SHADER_H_INCLUDED
