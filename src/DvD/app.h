#ifndef DVD_APP_H
#define DVD_APP_H

#include <string>

namespace app {
	extern std::string szConfigPath;

	extern bool disable_shaders;
	extern bool disable_sound;
	extern int max_texture_size;
	extern bool versus;
	extern bool fullscreen;
	extern int input_delay;

	void run();
}

#endif // DVD_APP_H
