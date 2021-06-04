#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "graphics.h"
#include "shader.h"
#include "util.h"

#include <glad/glad.h>

Shader::Shader() {
	program = 0;
	vertex = 0;
	fragment = 0;
}

Shader::~Shader() {
	if(graphics::shader_support) {
		glDeleteShader(vertex);
		glDeleteShader(fragment);
	}
}

void Shader::setInt(const char* variable, int value) const {
	GLint loc = glGetUniformLocation(program, variable);
	if(loc != -1) {
		glUniform1i(loc, value);
	}
}

void Shader::setFloat(const char* variable, float value) const {
	GLint loc = glGetUniformLocation(program, variable);
	if(loc != -1) {
		glUniform1f(loc, value);
	}
}

void Shader::setVec2(const char* variable, float value1, float value2) const {
	GLint loc = glGetUniformLocation(program, variable);
	if(loc != -1) {
		glUniform2f(loc, value1, value2);
	}
}

void Shader::setVec3(const char* variable, float value1, float value2, float value3) const {
	GLint loc = glGetUniformLocation(program, variable);
	if(loc != -1) {
		glUniform3f(loc, value1, value2, value3);
	}
}

void Shader::setVec4(const char* variable, float value1, float value2, float value3, float value4) const {
	GLint loc = glGetUniformLocation(program, variable);
	if(loc != -1) {
		glUniform4f(loc, value1, value2, value3, value4);
	}
}

void Shader::use() const {
	glUseProgram(program);
}

bool Shader::create(const char* szVertexFile, const char* szFragmentFile) {
	int nLines;
	char** szLines;
	int isCompiled;
	
	std::string vertexPath = util::getPath(szVertexFile);
	std::string fragmentPath = util::getPath(szFragmentFile);

	//Compile the vertex shader
	if(!(szLines = util::getLinesFromFile(&nLines, vertexPath))) {
		return false;
	}
	vertex = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertex, nLines, (const GLchar**)szLines, nullptr);
	glCompileShader(vertex);
	util::freeLines(szLines);

	glGetShaderiv(vertex, GL_COMPILE_STATUS, &isCompiled);
	if(!isCompiled) {
		int length = 0;
		glGetShaderiv(vertex, GL_INFO_LOG_LENGTH , &length);
		if(length > 1) {
			char* log = (char*)malloc(length);
			glGetShaderInfoLog(vertex, length, nullptr, log);
			fprintf(stderr, "GLSL: %s:\n%s\n", vertexPath.c_str(), log);
			free(log);
		}
		glDeleteShader(vertex);
		vertex = 0;
		return false;
	}

	//Compile the fragment shader
	if(!(szLines = util::getLinesFromFile(&nLines, fragmentPath))) {
		glDeleteShader(vertex);
		vertex = 0;
		return false;
	}
	fragment = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragment, nLines, (const GLchar**)szLines, nullptr);
	glCompileShader(fragment);
	util::freeLines(szLines);

	glGetShaderiv(fragment, GL_COMPILE_STATUS, &isCompiled);
	if(!isCompiled) {
		int length = 0;
		glGetShaderiv(fragment, GL_INFO_LOG_LENGTH, &length);
		if(length > 1) {
			char* log = (char*)malloc(length);
			glGetShaderInfoLog(fragment, length, nullptr, log);
			fprintf(stderr, "GLSL: %s:\n%s\n", fragmentPath.c_str(), log);
			free(log);
		}
		glDeleteShader(vertex);
		glDeleteShader(fragment);
		vertex = 0;
		fragment = 0;
		return false;
	}

	//Create the program
	program = glCreateProgram();
	glAttachShader(program, vertex);
	glAttachShader(program, fragment);
	glLinkProgram(program);

	glGetProgramiv(program, GL_LINK_STATUS, &isCompiled);
	if(!isCompiled) {
		int length = 0;
		glGetProgramiv(program, GL_INFO_LOG_LENGTH, &length);
		if(length > 1) {
			char* log = (char*)malloc(length);
			glGetProgramInfoLog(program, length, nullptr, log);
			fprintf(stderr, "GLSL: %s, %s\n%s\n", vertexPath.c_str(), fragmentPath.c_str(), log);
			free(log);
		}

		glDeleteProgram(program);
		glDeleteShader(vertex);
		glDeleteShader(fragment);
		program = 0;
		vertex = 0;
		fragment = 0;
		return false;
	}

	return true;
}
