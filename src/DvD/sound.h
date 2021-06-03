#ifndef SOUND_H_INCLUDED
#define SOUND_H_INCLUDED

#include "file.h"

namespace audio {
	/// @brief Initialize audio device for playback
	void init();
	/// @brief Deinitialize audio
	void deinit();
	/// @brief Apply audio effects with the game
	void refresh();

	extern bool enabled;

	/// @brief Sound class containing sample data and interacting with mixer
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

// #define AUDIO_STREAM_BUFFER_MAX 3

	/// @brief Music wrapper class around Sound
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

	/// @brief Speaker object that can only play one active voice
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
