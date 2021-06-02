#include "graphics.h"

#include "input.h"
#include "error.h"
#include "player.h"
#include "scene/scene.h"
#include "network.h"
#include "shader.h"
#include "stage.h"

#include "sys.h"

#ifndef __APPLE__
#define LOAD_GL_PROC(name, type) name = (PFN##type##PROC)SDL_GL_GetProcAddress(#name)
#define LOAD_GL_ARB_PROC(name, type, arbname) name = (PFN##type##PROC)SDL_GL_GetProcAddress(arbname)

#ifdef _WIN32
//1.3
// SDL_opengl.h already defines glActiveTexture and glBlendEquation
PFNGLACTIVETEXTUREPROC          glActiveTexture;
PFNGLBLENDEQUATIONPROC          glBlendEquation;
#endif

#ifdef GAME
PFNGLATTACHSHADERPROC			glAttachShader;
PFNGLCOMPILESHADERPROC			glCompileShader;
PFNGLCREATEPROGRAMPROC			glCreateProgram;
PFNGLDELETEPROGRAMPROC			glDeleteProgram;
PFNGLCREATESHADERPROC			glCreateShader;
PFNGLDELETESHADERPROC			glDeleteShader;
PFNGLGETPROGRAMINFOLOGPROC		glGetProgramInfoLog;
PFNGLGETPROGRAMIVPROC	        glGetProgramiv;
PFNGLGETPROGRAMINFOLOGPROC	    glGetShaderInfoLog;
PFNGLGETSHADERIVPROC			glGetShaderiv;
PFNGLGETUNIFORMLOCATIONPROC	 	glGetUniformLocation;
PFNGLLINKPROGRAMPROC 			glLinkProgram;
PFNGLSHADERSOURCEPROC 			glShaderSource;
PFNGLUNIFORM1IPROC 				glUniform1i;
PFNGLUNIFORM1FPROC 				glUniform1f;
PFNGLUNIFORM2FPROC 				glUniform2f;
PFNGLUNIFORM3FPROC 				glUniform3f;
PFNGLUNIFORM4FPROC 				glUniform4f;
PFNGLUSEPROGRAMPROC		 		glUseProgram;
#endif
#endif

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
		//1.2+ functions are not defined in Windows
		// SDL_opengl.h already defines glActiveTexture and glBlendEquation
		LOAD_GL_PROC(glActiveTexture, GLACTIVETEXTURE);
		LOAD_GL_PROC(glBlendEquation, GLBLENDEQUATION);
		if(!glBlendEquation)
            LOAD_GL_ARB_PROC(glBlendEquation, GLBLENDEQUATION, "glBlendEquationARB");
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
				LOAD_GL_PROC(glAttachShader, GLATTACHSHADER);
				LOAD_GL_PROC(glCompileShader, GLCOMPILESHADER);
				LOAD_GL_PROC(glCreateProgram, GLCREATEPROGRAM);
				LOAD_GL_PROC(glDeleteProgram, GLDELETEPROGRAM);
				LOAD_GL_PROC(glCreateShader, GLCREATESHADER);
				LOAD_GL_PROC(glDeleteShader, GLDELETESHADER);
				LOAD_GL_PROC(glGetProgramInfoLog, GLGETPROGRAMINFOLOG);
				LOAD_GL_PROC(glGetProgramiv, GLGETPROGRAMIV);
				LOAD_GL_PROC(glGetShaderInfoLog, GLGETSHADERINFOLOG);
				LOAD_GL_PROC(glGetShaderiv, GLGETSHADERIV);
				LOAD_GL_PROC(glGetUniformLocation, GLGETUNIFORMLOCATION);
				LOAD_GL_PROC(glLinkProgram, GLLINKPROGRAM);
				LOAD_GL_PROC(glShaderSource, GLSHADERSOURCE);
				LOAD_GL_PROC(glUniform1i, GLUNIFORM1I);
				LOAD_GL_PROC(glUniform1f, GLUNIFORM1F);
				LOAD_GL_PROC(glUniform2f, GLUNIFORM2F);
				LOAD_GL_PROC(glUniform3f, GLUNIFORM3F);
				LOAD_GL_PROC(glUniform4f, GLUNIFORM4F);
				LOAD_GL_PROC(glUseProgram, GLUSEPROGRAM);

                shader_support = glAttachShader && glCompileShader && glCreateProgram && glDeleteProgram &&
                                 glCreateShader && glDeleteShader && glGetProgramInfoLog && glGetProgramiv &&
                                 glGetShaderInfoLog && glGetShaderiv && glGetUniformLocation && glLinkProgram &&
                                 glShaderSource && glUniform1i && glUniform1f && glUniform2f && glUniform3f &&
                                 glUniform4f && glUseProgram;
			}
			if(!shader_support &&
                SDL_GL_ExtensionSupported("GL_ARB_shader_objects") &&
                SDL_GL_ExtensionSupported("GL_ARB_shading_language_100") &&
                SDL_GL_ExtensionSupported("GL_ARB_vertex_shader") &&
                SDL_GL_ExtensionSupported("GL_ARB_fragment_shader"))
            {
                LOAD_GL_ARB_PROC(glAttachShader, GLATTACHSHADER, "glAttachObjectARB");
                LOAD_GL_ARB_PROC(glCompileShader, GLCOMPILESHADER, "glCompileShaderARB");
                LOAD_GL_ARB_PROC(glCreateProgram, GLCREATEPROGRAM, "glCreateProgramObjectARB");
                LOAD_GL_ARB_PROC(glDeleteProgram, GLDELETEPROGRAM, "glDeleteObjectARB");
                LOAD_GL_ARB_PROC(glCreateShader, GLCREATESHADER, "glCreateShaderObjectARB");
                LOAD_GL_ARB_PROC(glDeleteShader, GLDELETESHADER, "glDeleteObjectARB");
                LOAD_GL_ARB_PROC(glGetProgramInfoLog, GLGETPROGRAMINFOLOG, "glGetInfoLogARB");
                LOAD_GL_ARB_PROC(glGetProgramiv, GLGETPROGRAMIV, "glGetObjectParameterivARB");
                LOAD_GL_ARB_PROC(glGetUniformLocation, GLGETUNIFORMLOCATION, "glGetUniformLocationARB");
				LOAD_GL_ARB_PROC(glLinkProgram, GLLINKPROGRAM, "glLinkProgramARB");
				LOAD_GL_ARB_PROC(glShaderSource, GLSHADERSOURCE, "glShaderSourceARB");
				LOAD_GL_ARB_PROC(glUniform1i, GLUNIFORM1I, "glUniform1iARB");
				LOAD_GL_ARB_PROC(glUniform1f, GLUNIFORM1F, "glUniform1fARB");
				LOAD_GL_ARB_PROC(glUniform2f, GLUNIFORM2F, "glUniform2fARB");
				LOAD_GL_ARB_PROC(glUniform3f, GLUNIFORM3F, "glUniform3fARB");
				LOAD_GL_ARB_PROC(glUniform4f, GLUNIFORM4F, "glUniform4fARB");
				LOAD_GL_ARB_PROC(glUseProgram, GLUSEPROGRAM, "glUseProgramObjectARB");

                shader_support = glAttachShader && glCompileShader && glCreateProgram && glDeleteProgram &&
                                 glCreateShader && glDeleteShader && glGetProgramInfoLog && glGetProgramiv &&
                                 glGetShaderiv && glGetUniformLocation && glLinkProgram && glShaderSource &&
                                 glUniform1i && glUniform1f && glUniform2f && glUniform3f && glUniform4f &&
                                 glUseProgram;
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
