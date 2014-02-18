#ifndef SOUND_H_INCLUDED
#define SOUND_H_INCLUDED

#include "globals.h"

#include "file.h"

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
		void createFromEmbed(File& file);
		bool exists();
		void destroy();

		float* samples;
		unsigned int c_samples;
		unsigned int sample_rate;
		int channels;
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

		Sound intro;
		Sound loop;
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

		void init();
		void play(Voice* voice);
		void stop();
	};
}

#endif // SOUND_H_INCLUDED
