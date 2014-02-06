#ifndef GRAPHICS_H_INCLUDED
#define GRAPHICS_H_INCLUDED

#include "globals.h"

#include "shader.h"

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
