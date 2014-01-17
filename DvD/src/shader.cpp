#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <GL/glew.h>

#include "graphics.h"
#include "shader.h"
#include "util.h"

Shader::Shader()
{
    shader_program = 0;
    shader_vertex = 0;
    shader_fragment = 0;
}

Shader::~Shader()
{
    if(Graphics::shader_support)
    {
        glDeleteShader(shader_vertex);
        glDeleteShader(shader_fragment);
    }
}

void Shader::setInt(const char* sz_variable_, int value_)
{
    GLint _loc = glGetUniformLocation(shader_program, sz_variable_);
    if(_loc != -1) glUniform1i(_loc, value_);
}

void Shader::setFloat(const char* sz_variable_, float value_)
{
    GLint _loc = glGetUniformLocation(shader_program, sz_variable_);
    if(_loc != -1) glUniform1f(_loc, value_);
}

void Shader::setVec2(const char* sz_variable_, float value1_, float value2_)
{
    GLint _loc = glGetUniformLocation(shader_program, sz_variable_);
    if(_loc != -1) glUniform2f(_loc, value1_, value2_);
}

void Shader::setVec3(const char* sz_variable_, float value1_, float value2_, float value3_)
{
    GLint _loc = glGetUniformLocation(shader_program, sz_variable_);
    if(_loc != -1) glUniform3f(_loc, value1_, value2_, value3_);
}

void Shader::setVec4(const char* sz_variable_, float value1_, float value2_, float value3_, float value4_)
{
    GLint _loc = glGetUniformLocation(shader_program, sz_variable_);
    if(_loc != -1) glUniform4f(_loc, value1_, value2_, value3_, value4_);
}

void Shader::use()
{
    glUseProgram(shader_program);
}

bool Shader::create(const char* szVertexFile, const char* szFragmentFile)
{
	int nLines;
	char** szLines;
    int isCompiled;

    //Compile the vertex shader
    if(!(szLines = getLinesFromFile(&nLines, szVertexFile)))
        return false;
    shader_vertex = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(shader_vertex, nLines, (const GLchar**)szLines, NULL);
    glCompileShader(shader_vertex);
    freeLines(szLines);

    glGetShaderiv(shader_vertex, GL_COMPILE_STATUS, &isCompiled);
    if(!isCompiled)
    {
#ifndef EMSCRIPTEN
        int length = 0;
        glGetShaderiv(shader_vertex, GL_INFO_LOG_LENGTH , &length);
        if(length > 1)
        {
            char* log = (char*)malloc(length);
            glGetInfoLogARB(shader_vertex, length, NULL, log);
            fprintf(stderr, "GLSL: %s:\n%s\n", szVertexFile, log);
            free(log);
        }
#endif
        glDeleteShader(shader_vertex);
        shader_vertex = 0;
        return false;
    }

    //Compile the fragment shader
    if(!(szLines = getLinesFromFile(&nLines, szFragmentFile)))
    {
        glDeleteShader(shader_vertex);
        shader_vertex = 0;
        return false;
    }
    shader_fragment = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(shader_fragment, nLines, (const GLchar**)szLines, NULL);
    glCompileShader(shader_fragment);
    freeLines(szLines);

    glGetShaderiv(shader_fragment, GL_COMPILE_STATUS, &isCompiled);
    if(!isCompiled)
    {
#ifndef EMSCRIPTEN
        int length = 0;
        glGetShaderiv(shader_fragment, GL_INFO_LOG_LENGTH, &length);
        if(length > 1)
        {
            char* log = (char*)malloc(length);
            glGetInfoLogARB(shader_vertex, length, NULL, log);
            fprintf(stderr, "GLSL: %s:\n%s\n", szFragmentFile, log);
            free(log);
        }
#endif
        glDeleteShader(shader_vertex);
        glDeleteShader(shader_fragment);
        shader_vertex = 0;
        shader_fragment = 0;
        return false;
    }

    //Create the program
    shader_program = glCreateProgram();
    glAttachShader(shader_program, shader_vertex);
    glAttachShader(shader_program, shader_fragment);
    glLinkProgram(shader_program);

    glGetProgramiv(shader_program, GL_LINK_STATUS, &isCompiled);
    if(!isCompiled)
    {
        int length = 0;
        glGetProgramiv(shader_program, GL_INFO_LOG_LENGTH, &length);
        if(length > 1)
        {
            char* log = (char*)malloc(length);
            glGetProgramInfoLog(shader_program, length, NULL, log);
            fprintf(stderr, "GLSL: %s, %s\n%s\n", szVertexFile, szFragmentFile, log);
            free(log);
        }

        glDeleteProgram(shader_program);
        glDeleteShader(shader_vertex);
        glDeleteShader(shader_fragment);
        shader_program = 0;
        shader_vertex = 0;
        shader_fragment = 0;
        return false;
    }

    return true;
}
