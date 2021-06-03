#include "graphics.h"

#include "input.h"
#include "error.h"
#include "player.h"
#include "scene/scene.h"
#include "network.h"
#include "shader.h"
#include "stage.h"
#include "sys.h"

#include <glad/glad.h>

//double oldTime = 0.0f;

// extern uint8_t* blankTex;

namespace graphics {
	constexpr auto FPS_BUFFER = 2;

	//State options
	int render = 0;

	int srcX = 0;
	int srcY = 0;
	int srcW = 0;
	int srcH = 0;

	float xscale = 1.0f;
	float yscale = 1.0f;

	//Immutable stuff
	unsigned int max_texture_size = 0;
	bool force_POT = false;
	bool shader_support = false;

	//Timer stuff
	unsigned long time = 0;

#ifdef SHOW_FPS
#define FPS_COUNTER_SIZE 64
	unsigned int tickValues[FPS_COUNTER_SIZE] = {0};
	unsigned int tickSum = 0;
	unsigned int tickIndex = 0;
#endif // SHOW_FPS

#ifdef GAME
	Shader shader_palette;
#endif

	void init(bool disable_shaders_, unsigned int max_texture_size_) {
        //REQUIRE VERSION 1.3
        GLint versionMajor, versionMinor;
        glGetIntegerv(GL_MAJOR_VERSION, &versionMajor);
        glGetIntegerv(GL_MINOR_VERSION, &versionMinor);
        if(versionMajor == 1 && versionMinor < 3) {
            error::die("Unsupported version of OpenGL: " + util::toString(versionMajor) + "." + util::toString(versionMinor));
        }

		//OPENGL
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		glOrtho(0, sys::WINDOW_WIDTH, sys::WINDOW_HEIGHT, 0, 0, 1);
		glMatrixMode(GL_MODELVIEW);

		glEnable(GL_TEXTURE_2D);
		glDisable(GL_DEPTH_TEST);
		glDisable(GL_LIGHTING);
		glEnable(GL_CULL_FACE);

		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		//Get max texture size
		glGetIntegerv(GL_MAX_TEXTURE_SIZE, (GLint*)&max_texture_size);
		if(max_texture_size_ && max_texture_size_ < max_texture_size) {
			max_texture_size = max_texture_size_;
		}

#ifdef _WIN32
        if(!glActiveTexture || !glBlendEquation) {
            error::die("Your OpenGL implementation isn't capable of running Dream vs. Dream.");
        }
#endif

#ifdef SPRTOOL
        (void)disable_shaders_;
#else
#ifdef __APPLE__
		//These functions are all available in OS X 10.6+
		shader_support = !disable_shaders_;
#else
		//Shader support?
		if(!disable_shaders_) {
            //See if we have opengl >= 2.0; this means we have non-arb
            //shading extensions
			GLint version;
			glGetIntegerv(GL_MAJOR_VERSION, &version);
			if(version >= 2) {
                shader_support = glAttachShader && glCompileShader && glCreateProgram && glDeleteProgram &&
                                 glCreateShader && glDeleteShader && glGetProgramInfoLog && glGetProgramiv &&
                                 glGetShaderInfoLog && glGetShaderiv && glGetUniformLocation && glLinkProgram &&
                                 glShaderSource && glUniform1i && glUniform1f && glUniform2f && glUniform3f &&
                                 glUniform4f && glUseProgram;
			}
		}
		//Load shaders, if possible
		if(shader_support) {
			shader_palette.create("shaders/vertex.v.glsl", "shaders/palette.f.glsl");
		}
#endif
#endif

		//Remove garbage data
		glClear(GL_COLOR_BUFFER_BIT);
	}

	void deinit() {
#ifndef SPRTOOL
		//delete [] blankTex;
#endif
	}

	int pixel = 2;
	int shift = 0;

	void refresh() {
#ifdef GAME
		if(Stage::stage == 3) {
			//Update pixel value
			if(pixel > 2) {
				pixel--;
			}

			if(FIGHT->round >= 1) {
				if(!util::roll(64)) {
					pixel = 2 + 2 * FIGHT->round;
				}
			}
			shift = 0;

			if(!SceneOptions::optionEpilepsy) {
				if(FIGHT->round >= 2) {
					if(!util::roll(64)) {
						shift = util::roll(1, 2);
					}
				}
			}
		}
#endif

		//Calculate fps, wait
		unsigned long delta = sys::getTime() - time;
		if(delta < sys::MSPF) {
			if(delta > FPS_BUFFER) {
				sys::sleep(sys::MSPF - delta);
			}
			for(;;) {
				if(sys::getTime() - time >= sys::MSPF) {
					break;
				}
			}
		}

#ifdef SHOW_FPS
		delta = sys::getTime() - time;
		tickSum -= tickValues[tickIndex];
		tickSum += delta;
		tickValues[tickIndex] = delta;
		if(++tickIndex >= FPS_COUNTER_SIZE) {
			tickIndex = 0;
		}
		char buff[256];
		sprintf8(buff, "[%f] " WINDOW_TITLE, 1000 / ((float)tickSum / FPS_COUNTER_SIZE));
		sys::setTitle(buff);
#endif // SHOW_FPS

		time = sys::getTime();

		glClear(GL_COLOR_BUFFER_BIT);
	}

	//State stuff
	void setClearColor(uint8_t r, uint8_t g, uint8_t b) {
		glClearColor(r / 255.0f, g / 255.0f, b / 255.0f, 1.0f);
	}

	void setColor(uint8_t r, uint8_t g, uint8_t b, float a) {
		glColor4f(r / 255.0f, g / 255.0f, b / 255.0f, a);
	}

	void setRect(int sX, int sY, int sW, int sH) {
		srcX = sX;
		srcY = sY;
		srcW = sW;
		srcH = sH;
	}

	void setScale(float scale_, float yscale_) {
		xscale = scale_;
		if(yscale_) {
			yscale = yscale_;
		} else {
			yscale = scale_;
		}
	}

	void setRender(int render_) {
		render = render_;
	}

#ifdef GAME
	void setPalette(unsigned int palette, float alpha, float r, float g, float b, float pct) {
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, palette);
		glActiveTexture(GL_TEXTURE0);

		shader_palette.use();

		shader_palette.setInt("texture", 0);
		shader_palette.setInt("palette", 1);

		if(Stage::stage == 3 && FIGHT->round >= 2) {
			shader_palette.setFloat("shift", shift / 256.0f);
		} else {
			shader_palette.setFloat("shift", 0.0f);
		}

		shader_palette.setVec3("color", r, g, b);
		shader_palette.setFloat("pct", pct);

		shader_palette.setFloat("alpha", alpha);

		if(Stage::stage == 3) {
			shader_palette.setInt("pixel", pixel);
		} else {
			shader_palette.setInt("pixel", 1);
		}
	}
#endif
}
