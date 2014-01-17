#include <stdlib.h>
#include <string.h>
#include <stddef.h>
#include <stdio.h>
#include <math.h>

#ifndef _WIN32
#include <sys/stat.h>
#endif

#ifndef NO_SOUND
#ifndef EMSCRIPTEN
#include <portaudio.h>
#include <sndfile.h>
#endif
#endif

#include "sound.h"
#include "error.h"
#include "globals.h"
#include "menu.h"
#include "stage.h"

#define SAMPLE_RATE 44100

#define SOUND_SOURCE_MAX 64
#define SPEAKER_SOURCE_MAX 8

bool Sound::enabled = false;

static Music* music = NULL;

#ifndef NO_SOUND

#ifdef EMSCRIPTEN
Sound* sound_channels[SOUND_SOURCE_MAX] = {NULL};
void sound_channelDone(int channel)
{
    sound_channels[channel]->channel = -1;
    sound_channels[channel] = NULL;
}

void sound_musicDone()
{
    if(music)
    {
	Mix_HaltMusic();
	Mix_PlayMusic(music->loop, -1);
    }
}
#else

class SoundSource
{
public:
    Sound* sound;
    double i_sample;
    float frequency;

    SoundSource()
    {
        i_sample = 0.0;
        frequency = 1.0f;
        sound = NULL;
    }
};

class SpeakerSource : public SoundSource
{
public:
    Speaker* speaker;

    SpeakerSource() : SoundSource()
    {
        speaker = NULL;
    }
};

#if 0
class MusicStream
{
public:
    SNDFILE* stream;

    unsigned int c_samples;
    unsigned int sample_rate;
    int channels;

    MusicStream()
    {
        stream = NULL;
        c_samples = 0;
        sample_rate = 0;
        channels = 0;
    }
};
#endif

static SoundSource sound_sources[SOUND_SOURCE_MAX];
static SpeakerSource speaker_sources[SPEAKER_SOURCE_MAX];

//#define MUSIC_BUFFER_SIZE 1024

//static float music_samples[MUSIC_BUFFER_SIZE];
static double i_music_sample = 0;
static bool music_is_loop = false;
float music_frequency = 1.0f;
//static bool music_end = false;

//static MusicStream stream_intro;
//static MusicStream stream_loop;

static PaStream* audio_stream = NULL;

static int portaudioCallback(const void* b_input_, void* b_output_,
                             unsigned long frames_per_buffer_,
                             const PaStreamCallbackTimeInfo* time_info_,
                             PaStreamCallbackFlags status_flags_,
                             void* data_)
{
    float music_volume = optionMusVolume / (float)200;
    float sound_volume = optionSfxVolume / (float)100;
    float voice_volume = optionVoiceVolume / (float)100;

    float* out = (float*)b_output_;

    for(unsigned int i = 0; i < frames_per_buffer_; i++)
    {
        out[0] = 0.0f;
        out[1] = 0.0f;

        if(music)
        {
            Sound* sound;
            if(music_is_loop)
                sound = &music->loop;
            else
                sound = &music->intro;

            if(sound->channels == 1)
            {
                out[0] = sound->samples[(int)i_music_sample] * music_volume;
                out[1] = sound->samples[(int)i_music_sample] * music_volume;
            }
            else if(sound->channels == 2)
            {
                out[0] = sound->samples[((int)i_music_sample) * 2] * music_volume;
                out[1] = sound->samples[((int)i_music_sample) * 2 + 1] * music_volume;
            }

            i_music_sample += (sound->sample_rate / (float)SAMPLE_RATE) * music_frequency;
            if(i_music_sample >= sound->c_samples)
            {
                if(menu == MENU_VERSUS)
                    music = NULL;
                else
                {
                    music_is_loop = true;
                    i_music_sample = 0.0;
                }
            }
        }

        for(int j = 0; j < SOUND_SOURCE_MAX; j++)
        {
            Sound* sound = sound_sources[j].sound;
            if(sound)
            {
                if(sound->channels == 1)
                {
                    out[0] += sound->samples[(int)sound_sources[j].i_sample] * sound_volume;
                    out[1] += sound->samples[(int)sound_sources[j].i_sample] * sound_volume;
                }
                else if(sound->channels == 2)
                {
                    out[0] += sound->samples[((int)sound_sources[j].i_sample) * 2] * sound_volume;
                    out[1] += sound->samples[((int)sound_sources[j].i_sample) * 2 + 1] * sound_volume;
                }

                //Increase sound buffer counters
                sound_sources[j].i_sample += (sound->sample_rate / (float)SAMPLE_RATE) * sound_sources[j].frequency;
                if(sound_sources[j].i_sample >= sound->c_samples)
                {
                    sound_sources[j].sound = NULL;
                }
            }
        }

        for(int j = 0; j < SPEAKER_SOURCE_MAX; j++)
        {
            Sound* sound = speaker_sources[j].sound;
            if(sound)
            {
                if(sound->channels == 1)
                {
                    out[0] += sound->samples[(int)speaker_sources[j].i_sample] * voice_volume;
                    out[1] += sound->samples[(int)speaker_sources[j].i_sample] * voice_volume;
                }
                else if(sound->channels == 2)
                {
                    out[0] += sound->samples[((int)speaker_sources[j].i_sample) * 2] * voice_volume;
                    out[1] += sound->samples[((int)speaker_sources[j].i_sample) * 2 + 1] * voice_volume;
                }

                //Increase sound buffer counters
                speaker_sources[j].i_sample += (sound->sample_rate / (float)SAMPLE_RATE) * speaker_sources[j].frequency;
                if(speaker_sources[j].i_sample >= sound->c_samples)
                {
                    speaker_sources[j].sound = NULL;
                }
            }
        }
        out += 2;
    }
    return 0;
}

#endif //EMSCRIPTEN
#endif //NO_SOUND

void Sound::init()
{
#ifndef NO_SOUND
#ifdef EMSCRIPTEN
//    if(Mix_OpenAudio(48000, AUDIO_S16, 2, SOUND_SOURCE_MAX))
    if(Mix_OpenAudio(0, 0, 0, 0))
    {
	error("SDL_mixer could not initialize.");
	return;
    }
    Mix_ChannelFinished(sound_channelDone);
    Mix_HookMusicFinished(sound_musicDone);
#else
    int _error = Pa_Initialize();
    if(_error != paNoError)
    {
        error("PortAudio could not initialize: " + std::string(Pa_GetErrorText(_error)));
        return;
    }

    _error = Pa_OpenDefaultStream(&audio_stream, 0, 2, paFloat32, SAMPLE_RATE, paFramesPerBufferUnspecified, portaudioCallback, NULL);
    if(_error != paNoError)
    {
        error("PortAudio could not open the default stream: %s" + std::string(Pa_GetErrorText(_error)));
        return;
    }

    Pa_StartStream(audio_stream);

    Sound::enabled = true;
#endif
#endif
}

void Sound::deinit()
{
#ifndef NO_SOUND
#ifdef EMSCRIPTEN
    if(Sound::enabled)
	Mix_CloseAudio();
#else
    if(Sound::enabled)
    {
        Pa_AbortStream(audio_stream);
        Pa_Terminate();
    }
#endif
#endif
}

void Sound::refresh()
{
#ifndef NO_SOUND
#ifndef EMSCRIPTEN
    if(menu == MENU_FIGHT && stage == 3)
    {
        float amplitude = FIGHT->round * 0.1;
        music_frequency = 1.0f + rollf() * amplitude - amplitude / 2;
    }
    else
    {
        music_frequency = 1.0f;
    }
#endif
#endif
}

Sound::Sound()
{
#ifdef EMSCRIPTEN
    sound = NULL;
    channel = -1;
#else
    samples = NULL;
    c_samples = 0;
    sample_rate = 0;
    channels = 0;
#endif
}

Sound::~Sound()
{
#ifdef EMSCRIPTEN
    if(sound)
	Mix_FreeChunk(sound);
#else
    if(samples)
        free(samples);
#endif
}

void Sound::play()
{
    play(1.0f);
}

void Sound::play(float freq)
{
#ifndef NO_SOUND
#ifdef EMSCRIPTEN
    channel = Mix_PlayChannel(-1, sound, 0);
    Mix_Volume(channel, optionSfxVolume * MIX_MAX_VOLUME / 100);
#else
    for(int i = 0; i < SOUND_SOURCE_MAX; i++)
    {
        if(!sound_sources[i].sound)
        {
            sound_sources[i].sound = this;
            sound_sources[i].i_sample = 0.0;
            sound_sources[i].frequency = freq;
            break;
        }
    }
#endif
#endif
}

void Sound::stop()
{
#ifndef NO_SOUND
#ifdef EMSCRIPTEN
    if(channel != -1)
    {
	Mix_HaltChannel(channel);
	channel = -1;
    }
#else
    for(int i = 0; i < SOUND_SOURCE_MAX; i++)
    {
        if(sound_sources[i].sound == this)
        {
            sound_sources[i].sound = NULL;
            break;
        }
    }
#endif
#endif
}

bool Sound::playing()
{
#ifndef NO_SOUND
#ifdef EMSCRIPTEN
    return channel != -1;
#else
    for(int i = 0; i < SOUND_SOURCE_MAX; i++)
    {
        if(sound_sources[i].sound == this)
        {
            return true;
        }
    }
#endif
#endif
    return false;
}

void Sound::createFromFile(std::string szFileName)
{
#ifndef NO_SOUND
#ifdef EMSCRIPTEN
    if(!(sound = Mix_LoadWAV(szFileName.c_str())))
        error("Unable to load audio file \"" + szFileName + "\".");
#else
    if(Sound::enabled)
    {
        //Open stream
        SNDFILE* _stream = NULL;
        SF_INFO _info;
        //memset(&_info, 0, sizeof(_info));

        FILE* _fp = fopen8(szFileName, "rb");
        if(!_fp)
            goto error;
        _stream = sf_open_fd(fileno(_fp), SFM_READ, &_info, SF_FALSE);

        if(!_stream)
            goto error;

        //Get size of buffer and allocate memory
        c_samples = _info.frames;
        channels = _info.channels;
        sample_rate = _info.samplerate;
        samples = new float[c_samples * channels];

        if(sf_readf_float(_stream, samples, c_samples) != c_samples)
            goto error;
        goto end;

error:
        error("Unable to load audio file \"" + szFileName + "\".");
        if(samples)
            delete [] samples;
        c_samples = 0;
        channels = 0;

end:
        if(_stream)
            sf_close(_stream);
        if(_fp)
            fclose(_fp);
    }
#endif
#endif
}

bool Sound::exists()
{
#ifndef NO_SOUND
#ifdef EMSCRIPTEN
    return sound != NULL;
#else
    return samples != NULL;
#endif
#else
    return true; //don't generate errors
#endif
}

void Sound::destroy()
{
#ifndef NO_SOUND
#ifdef EMSCRIPTEN
    if(sound)
	Mix_FreeChunk(sound);
#else
    if(samples)
        free(samples);
#endif
#endif
}

//Music
Music::Music()
{
#ifdef EMSCRIPTEN
	intro = NULL;
	loop = NULL;
#endif
}

Music::~Music()
{
}

bool fileExists(const char* sz_file)
{
#ifndef NO_SOUND
#ifdef _WIN32
	wchar_t* sz_file16 = utf8to16(sz_file);
    DWORD dwAttrib = GetFileAttributesW((LPCWSTR)sz_file16);
	free(sz_file16);
    return (dwAttrib != INVALID_FILE_ATTRIBUTES && !(dwAttrib & FILE_ATTRIBUTE_DIRECTORY));
#else
    struct stat sts;
    return !(stat((const char*)sz_file, &sts) == -1 && errno == ENOENT);
#endif
#else
    return true; //don't generate errors
#endif
}

void Music::play()
{
#ifndef NO_SOUND
#ifdef EMSCRIPTEN
    Mix_HaltMusic();
    music = this;
    //if(intro)
	//Mix_PlayMusic(intro, 0);
    //else
	Mix_PlayMusic(loop, -1);
#else
    music = NULL;
    if(Sound::enabled)
    {
#if 0
        if(stream_intro.stream)
        {
            sf_close(stream_intro.stream);
            stream_intro.stream = NULL;
        }
        if(stream_loop.stream)
        {
            sf_close(stream_loop.stream);
            stream_loop.stream = NULL;
        }

        music_loop = true;
        music_end = false;

        //Open stream
        SF_INFO _info;
        //memset(&_info, 0, sizeof(_info));

        if(fileExists(intro))
        {
            FILE* _fp = FOPEN(intro, "rb");
            if(_fp)
            {
                stream_intro.stream = sf_open_fd(fileno(_fp), SFM_READ, &_info, SF_TRUE);
                if(stream_intro.stream)
                {
                    stream_intro.c_samples = _info.frames;
                    stream_intro.sample_rate = _info.samplerate;
                    stream_intro.channels = _info.channels;

                    music_loop = false;
                }
            }
        }

        if(fileExists(loop))
        {
            FILE* _fp = FOPEN(loop, "rb");
            if(_fp)
            {
                stream_loop.stream = sf_open_fd(fileno(_fp), SFM_READ, &_info, SF_TRUE);
                if(stream_loop.stream)
                {
                    stream_loop.c_samples = _info.frames;
                    stream_loop.sample_rate = _info.samplerate;
                    stream_loop.channels = _info.channels;
                }
            }
        }
#endif

        music_is_loop = true;

        if(intro.exists())
            music_is_loop = false;

        music = this;
        i_music_sample = 0.0;
    }
#endif
#else
	music = this;
#endif
}

void Music::stop()
{
    music = NULL;
#ifdef EMSCRIPTEN
    Mix_HaltMusic();
#endif
}

void Music::createFromFile(std::string szIntro_, std::string szLoop_)
{
#ifndef NO_SOUND
#ifdef EMSCRIPTEN
	if(szIntro_.length())
		intro = Mix_LoadMUS(szIntro_.c_str());
	if(szLoop_.length())
		loop = Mix_LoadMUS(szLoop_.c_str());
#else
	if(szIntro_.length())
		intro.createFromFile(szIntro_);
	if(szLoop_.length())
		loop.createFromFile(szLoop_);
#endif
#endif
}

bool Music::exists()
{
#ifndef NO_SOUND
#ifdef EMSCRIPTEN
    return loop != NULL;
#else
    return loop.exists();
#endif
#else
    return true; //don't generate errors
#endif
}

bool Music::isPlaying()
{
    return music == this;
}

//Voice
#if 0
Voice::Voice()
{
}

Voice::~Voice()
{
}

void Voice::createFromFile(const char* filename)
{
    sound.createFromFile(filename);
}

bool Voice::exists()
{
    return sound.exists();
}
#endif

//Speaker
Speaker::Speaker()
{
}

Speaker::~Speaker()
{
}

void Speaker::init()
{
#ifndef NO_SOUND
#ifdef EMSCRIPTEN
    //Dirty dirty hack, but for the most part it shouldn't cause noticable issues
    static int lastChannel = SOUND_SOURCE_MAX - 1;
    channel = --lastChannel;
#else
    for(int i = 0; i < SPEAKER_SOURCE_MAX; i++)
    {
        if(!speaker_sources[i].speaker)
        {
            speaker_sources[i].speaker = this;
            break;
        }
    }
#endif
#endif
}

void Speaker::play(Voice* voice, bool randomize)
{
#ifndef NO_SOUND
#ifdef EMSCRIPTEN
    //Mix_PlayChannel(channel, voice->sound, 0);
    Mix_Volume(Mix_PlayChannel(-1, voice->sound, 0), optionVoiceVolume * MIX_MAX_VOLUME / 100);
#else
    for(int i = 0; i < SPEAKER_SOURCE_MAX; i++)
    {
        if(speaker_sources[i].speaker == this)
        {
            speaker_sources[i].sound = (Sound*)voice;
            speaker_sources[i].i_sample = 0.0;

            //float freq = 1.0f;
            //if(randomize)
                float freq = 1.0f + rollf() * 0.1f - 0.05;
            speaker_sources[i].frequency = freq;
            break;
        }
    }
#endif
#endif
}

void Speaker::stop()
{
#ifndef NO_SOUND
#ifdef EMSCRIPTEN
    //Mix_HaltChannel(channel);
#else
    for(int i = 0; i < SPEAKER_SOURCE_MAX; i++)
    {
        if(speaker_sources[i].speaker == this)
        {
            speaker_sources[i].sound = NULL;
        }
    }
#endif
#endif
}
