#ifndef GRAPHICS_H_INCLUDED
#define GRAPHICS_H_INCLUDED

#include "globals.h"

#ifndef COMPILER // Assume Compiler does not need to know about SDL_opengl.h
#include <SDL_opengl.h>
#endif

#include "shader.h"

#ifndef __APPLE__
#ifdef _WIN32
#ifndef COMPILER
//1.3
// SDL_opengl.h already defines glActiveTexture and glBlendEquation
// Temporary hack
#define glActiveTexture procGlActiveTexture
#define glBlendEquation procGlBlendEquation
extern PFNGLACTIVETEXTUREPROC           glActiveTexture;
extern PFNGLBLENDEQUATIONPROC           glBlendEquation;
#endif
#endif

#ifdef GAME
extern PFNGLATTACHSHADERPROC			glAttachShader;
extern PFNGLCOMPILESHADERPROC			glCompileShader;
extern PFNGLCREATEPROGRAMPROC			glCreateProgram;
extern PFNGLDELETEPROGRAMPROC			glDeleteProgram;
extern PFNGLCREATESHADERPROC			glCreateShader;
extern PFNGLDELETESHADERPROC			glDeleteShader;
extern PFNGLGETPROGRAMINFOLOGPROC		glGetProgramInfoLog;
extern PFNGLGETPROGRAMIVPROC			glGetProgramiv;
extern PFNGLGETPROGRAMINFOLOGPROC		glGetShaderInfoLog;
extern PFNGLGETSHADERIVPROC				glGetShaderiv;
extern PFNGLGETUNIFORMLOCATIONPROC	 	glGetUniformLocation;
extern PFNGLLINKPROGRAMPROC 			glLinkProgram;
extern PFNGLSHADERSOURCEPROC 			glShaderSource;
extern PFNGLUNIFORM1IPROC 				glUniform1i;
extern PFNGLUNIFORM1FPROC 				glUniform1f;
extern PFNGLUNIFORM2FPROC 				glUniform2f;
extern PFNGLUNIFORM3FPROC 				glUniform3f;
extern PFNGLUNIFORM4FPROC 				glUniform4f;
extern PFNGLUSEPROGRAMPROC		 		glUseProgram;
#endif
#endif

namespace graphics {
	//Init/deinit
	void init(bool disable_shaders, unsigned int max_texture_size);
	void deinit();

	void refresh();

	void setClearColor(ubyte_t r, ubyte_t g, ubyte_t b);
	void setRect(int sX, int sY, int sW, int sH);
	void setColor(ubyte_t r, ubyte_t g, ubyte_t b, float a = 1.0f);
	void setScale(float scale, float yscale = 0.0f);
	void setRender(int render);

	void setPalette(unsigned int palette, float alpha, float r, float g, float b, float pct);

	//Immutable stuff
	extern unsigned int max_texture_size;
	//extern bool force_POT;
	extern bool shader_support;

	//Shaders
	extern Shader shader_palette;

	extern int render;
	extern int srcX, srcY, srcW, srcH;
	extern float xscale, yscale;
	extern unsigned int shader;
	extern unsigned int palette;
}

#endif // GRAPHICS_H_INCLUDED
