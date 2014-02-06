#ifndef SOUND_H_INCLUDED
#define SOUND_H_INCLUDED

#include "globals.h"

namespace audio {
	void init();
	void deinit();
	void refresh();

	extern bool enabled;

	class Sound {
	public:
		Sound();
		~Sound();
		void play();
		void play(float freq);
		void stop();

		bool playing();

		void createFromFile(const std::string& filename);
		bool exists();
		void destroy();

#ifdef EMSCRIPTEN
		Mix_Chunk* sound;
		int channel;
#else
		float* samples;
		unsigned int c_samples;
		unsigned int sample_rate;
		int channels;
#endif
	};

#define AUDIO_STREAM_BUFFER_MAX 3

	class Music {
	public:
		Music();
		~Music();

		void play();

		static void stop();

		void createFromFile(const std::string& szIntro, const std::string& szLoop);
		bool exists();

		bool isPlaying();

#ifdef EMSCRIPTEN
		Mix_Music* intro;
		Mix_Music* loop;
#else
		Sound intro;
		Sound loop;
#endif
	};

	/*
	class Voice
	{
	public:
	    Voice();
	    ~Voice();

	    void createFromFile(const char* filename);
	    bool exists();

	    Sound sound;
	};
	*/

	typedef Sound Voice;

	class Speaker {
	public:
		Speaker();
		~Speaker();

#ifdef EMSCRIPTEN
		int channel;
#endif

		void init();
		void play(Voice* voice);
		void stop();
	};
}

#endif // SOUND_H_INCLUDED
