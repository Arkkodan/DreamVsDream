#include "sound.h"

#include "../util/fileIO.h"
#include "../util/rng.h"
#include "error.h"
#include "scene/fight.h"
#include "scene/options.h"
#include "scene/scene.h"
#include "stage.h"

#ifndef _WIN32
#include <sys/stat.h>
#endif

#ifndef NO_SOUND
#include <SDL_audio.h>
#include <sndfile.h>
#endif

namespace audio {
  static constexpr auto SAMPLE_RATE = 44100;

  static constexpr auto SOUND_SOURCE_MAX = 64;
  static constexpr auto SPEAKER_SOURCE_MAX = 8;

  static SDL_AudioSpec audioSpec;
  static bool enabled = false;

  static const Music *music = nullptr;

#ifndef NO_SOUND

  struct SoundSource {
    const Sound *sound;
    double i_sample;
    float frequency;

    SoundSource() {
      i_sample = 0.0;
      frequency = 1.0f;
      sound = nullptr;
    }
  };

  struct SpeakerSource : public SoundSource {
    Speaker *speaker;

    SpeakerSource() : SoundSource() { speaker = nullptr; }
  };

#if 0
  class MusicStream {
  public:
    SNDFILE *stream;

    unsigned int c_samples;
    unsigned int sample_rate;
    int channels;

    MusicStream() {
      stream = nullptr;
      c_samples = 0;
      sample_rate = 0;
      channels = 0;
    }
  };
#endif

  static SoundSource sound_sources[SOUND_SOURCE_MAX];
  static SpeakerSource speaker_sources[SPEAKER_SOURCE_MAX];

  //#define MUSIC_BUFFER_SIZE 1024

  // static float music_samples[MUSIC_BUFFER_SIZE];
  static double i_music_sample = 0;
  static bool music_is_loop = false;
  static float music_frequency = 1.0f;
  // static bool music_end = false;

  // static MusicStream stream_intro;
  // static MusicStream stream_loop;

  static void audioCallback(void *udata, unsigned char *stream, int _size) {
    (void)udata;

    float music_volume = scene::Options::getMusVolume() / (float)200;
    float sound_volume = scene::Options::getSfxVolume() / (float)100;
    float voice_volume = scene::Options::getVoiceVolume() / (float)100;

    float *out = (float *)stream;
    unsigned int size = _size / 4 / audioSpec.channels;

    int sceneIndex = scene::getSceneIndex();

    for (unsigned int i = 0; i < size; i++) {
      out[0] = 0.0f;
      out[1] = 0.0f;

      if (music) {
        const Sound *sound;
        if (music_is_loop) {
          sound = music->getcLoop();
        }
        else {
          sound = music->getcIntro();
        }

        int channels = sound->getChannelCount();
        const auto &samples = sound->getcrSamples();
        if (channels == 1) {
          out[0] = samples[(int)i_music_sample] * music_volume;
          out[1] = samples[(int)i_music_sample] * music_volume;
        }
        else if (channels == 2) {
          out[0] = samples[((int)i_music_sample) * 2] * music_volume;
          out[1] = samples[((int)i_music_sample) * 2 + 1] * music_volume;
        }

        i_music_sample +=
            (static_cast<double>(sound->getSampleRate()) / (float)SAMPLE_RATE) *
            music_frequency;
        if (i_music_sample >= sound->getSampleCount()) {
          if (sceneIndex == scene::SCENE_VERSUS) {
            music = nullptr;
          }
          else {
            music_is_loop = true;
            i_music_sample = 0.0;
          }
        }
      }

      int stage = Stage::getStageIndex();
      for (int j = 0; j < SOUND_SOURCE_MAX; j++) {
        const Sound *sound = sound_sources[j].sound;
        if (sound) {
          int channels = sound->getChannelCount();
          const auto &samples = sound->getcrSamples();
          if (sceneIndex == scene::SCENE_FIGHT && stage == 3) {
            if (channels == 1) {
              out[0] += samples[(int)sound_sources[j].i_sample / 8 * 8] *
                        sound_volume;
              out[1] += samples[(int)sound_sources[j].i_sample / 8 * 8] *
                        sound_volume;
            }
            else if (channels == 2) {
              out[0] += samples[((int)sound_sources[j].i_sample) / 8 * 8 * 2] *
                        sound_volume;
              out[1] +=
                  samples[((int)sound_sources[j].i_sample) / 8 * 8 * 2 + 1] *
                  sound_volume;
            }
          }
          else {
            if (channels == 1) {
              out[0] += samples[(int)sound_sources[j].i_sample] * sound_volume;
              out[1] += samples[(int)sound_sources[j].i_sample] * sound_volume;
            }
            else if (channels == 2) {
              out[0] +=
                  samples[((int)sound_sources[j].i_sample) * 2] * sound_volume;
              out[1] += samples[((int)sound_sources[j].i_sample) * 2 + 1] *
                        sound_volume;
            }
          }

          // Increase sound buffer counters
          sound_sources[j].i_sample +=
              (static_cast<double>(sound->getSampleRate()) /
               (float)SAMPLE_RATE) *
              sound_sources[j].frequency;
          if (sound_sources[j].i_sample >= sound->getSampleCount()) {
            sound_sources[j].sound = nullptr;
          }
        }
      }

      for (int j = 0; j < SPEAKER_SOURCE_MAX; j++) {
        const Sound *sound = speaker_sources[j].sound;
        if (sound) {
          int channels = sound->getChannelCount();
          const auto &samples = sound->getcrSamples();
          if (sceneIndex == scene::SCENE_FIGHT && stage == 3) {
            if (channels == 1) {
              out[0] += samples[(int)speaker_sources[j].i_sample / 8 * 8] *
                        voice_volume;
              out[1] += samples[(int)speaker_sources[j].i_sample / 8 * 8] *
                        voice_volume;
            }
            else if (channels == 2) {
              out[0] +=
                  samples[((int)speaker_sources[j].i_sample) / 8 * 8 * 2] *
                  voice_volume;
              out[1] +=
                  samples[((int)speaker_sources[j].i_sample) / 8 * 8 * 2 + 1] *
                  voice_volume;
            }
          }
          else {
            if (channels == 1) {
              out[0] +=
                  samples[(int)speaker_sources[j].i_sample] * voice_volume;
              out[1] +=
                  samples[(int)speaker_sources[j].i_sample] * voice_volume;
            }
            else if (channels == 2) {
              out[0] += samples[((int)speaker_sources[j].i_sample) * 2] *
                        voice_volume;
              out[1] += samples[((int)speaker_sources[j].i_sample) * 2 + 1] *
                        voice_volume;
            }
          }

          // Increase sound buffer counters
          speaker_sources[j].i_sample +=
              (static_cast<double>(sound->getSampleRate()) /
               (float)SAMPLE_RATE) *
              speaker_sources[j].frequency;
          if (speaker_sources[j].i_sample >= sound->getSampleCount()) {
            speaker_sources[j].sound = nullptr;
          }
        }
      }
      out += 2;
    }
  }

#endif // NO_SOUND

  void init() {
#ifndef NO_SOUND
    SDL_AudioSpec want;
    SDL_zero(want);
    want.freq = 44100;
    want.format = AUDIO_F32;
    want.channels = 2;
    want.samples = 1024;
    want.callback = audioCallback;

    if (SDL_OpenAudio(&want, &audioSpec) < 0) {
      error::error("SDL could not open audio: " + std::string(SDL_GetError()));
      return;
    }

    SDL_PauseAudio(0);

    enabled = true;
#endif
  }

  void deinit() {
#ifndef NO_SOUND
    if (enabled) {
      SDL_CloseAudio();
    }
#endif
  }

  void refresh() {
#ifndef NO_SOUND
    if (scene::getSceneIndex() == scene::SCENE_FIGHT &&
        Stage::getStageIndex() == 3) {
      float amplitude = FIGHT->getRound() * 0.1f;
      music_frequency = 1.0f + util::rollf() * amplitude - amplitude / 2;
    }
    else {
      music_frequency = 1.0f;
    }
#endif
  }

  Sound::Sound() {
    samples.clear();
    c_samples = 0;
    sample_rate = 0;
    channels = 0;
  }

  Sound::~Sound() {}

  void Sound::play() const { play(1.0f); }

  void Sound::play(float freq) const {
#ifndef NO_SOUND
    for (int i = 0; i < SOUND_SOURCE_MAX; i++) {
      if (!sound_sources[i].sound) {
        sound_sources[i].sound = this;
        sound_sources[i].i_sample = 0.0;
        sound_sources[i].frequency = freq;
        break;
      }
    }
#endif
  }

  void Sound::stop() const {
#ifndef NO_SOUND
    for (int i = 0; i < SOUND_SOURCE_MAX; i++) {
      if (sound_sources[i].sound == this) {
        sound_sources[i].sound = nullptr;
        break;
      }
    }
#endif
  }

  bool Sound::playing() const {
#ifndef NO_SOUND
    for (int i = 0; i < SOUND_SOURCE_MAX; i++) {
      if (sound_sources[i].sound == this) {
        return true;
      }
    }
#endif
    return false;
  }

  void Sound::createFromFile(const std::string &szFileName) {
#ifndef NO_SOUND
    if (enabled) {
      bool err = false;

      // Open stream
      SNDFILE *stream = nullptr;
      SF_INFO info;
      // memset(&info, 0, sizeof(_info));

      std::string path = util::getPath(szFileName);
      FILE *fp = util::ufopen(path, "rb");
      if (!fp) {
        goto error;
      }
      stream = sf_open_fd(fileno(fp), SFM_READ, &info, SF_FALSE);

      if (!stream) {
        goto error;
      }

      // Get size of buffer and allocate memory
      c_samples = static_cast<unsigned int>(info.frames);
      channels = info.channels;
      sample_rate = info.samplerate;
      samples.resize(c_samples * channels);

      if (sf_readf_float(stream, samples.data(), c_samples) != c_samples) {
        goto error;
      }
      goto end;

    error:
      samples.clear();
      c_samples = 0;
      channels = 0;
      err = true;

    end:
      if (stream) {
        sf_close(stream);
      }
      if (fp) {
        fclose(fp);
      }

      if (err) {
        error::die("Unable to load audio file \"" + path + "\".");
      }
    }
#endif
  }

  const std::vector<float> &Sound::getcrSamples() const { return samples; }
  unsigned int Sound::getSampleCount() const { return c_samples; }
  unsigned int Sound::getSampleRate() const { return sample_rate; }
  int Sound::getChannelCount() const { return channels; }

  /*
   * CREATE FROM EMBED
   */

  struct vio_Stream {
    sf_count_t origin;
    sf_count_t size;
    File *file;
  };

  static sf_count_t vio_filelen(void *data) {
    vio_Stream *stream = (vio_Stream *)data;
    return stream->size;
  }

  static sf_count_t vio_seek(sf_count_t offset, int whence, void *data) {
    vio_Stream *stream = (vio_Stream *)data;
    switch (whence) {
    case SEEK_CUR:
      stream->file->seek(static_cast<long>(stream->file->tell() + offset));
      return stream->file->tell() - stream->origin;
    case SEEK_SET:
      stream->file->seek(static_cast<long>(stream->origin + offset));
      return offset;
    case SEEK_END:
      stream->file->seek(
          static_cast<long>(stream->size + stream->origin + offset));
      return stream->file->tell() - stream->origin;
    }
    return 0;
  }

  static sf_count_t vio_read(void *ptr, sf_count_t count, void *data) {
    vio_Stream *stream = (vio_Stream *)data;

    if (stream->file->tell() + count > stream->origin + stream->size)
      count = (stream->origin + stream->size) - stream->file->tell();

    stream->file->read(ptr, static_cast<size_t>(count));
    return count;
  }

  static sf_count_t vio_write(const void *ptr, sf_count_t count, void *data) {
    (void)ptr;
    (void)count;
    (void)data;
    return 0;
  }

  static sf_count_t vio_tell(void *data) {
    vio_Stream *stream = (vio_Stream *)data;
    return stream->file->tell() - stream->origin;
  }

  SF_VIRTUAL_IO sfio = {
      vio_filelen, vio_seek, vio_read, vio_write, vio_tell,
  };

  void Sound::createFromEmbed(File &file) {
    uint32_t size = file.readDword();

    SNDFILE *stream = NULL;
    SF_INFO info;

    vio_Stream vio = {file.tell(), size, &file};

    // Open stream
    if (!(stream = sf_open_virtual(&sfio, SFM_READ, &info, &vio)))
      goto end;

    // Read audio data
    c_samples = static_cast<unsigned int>(info.frames);
    channels = info.channels;
    sample_rate = info.samplerate;
    samples.resize(c_samples * channels);

    if (sf_readf_float(stream, samples.data(), c_samples) != c_samples) {
      goto error;
    }
    goto end;
  error:
    samples.clear();
    c_samples = 0;
    channels = 0;

  end:
    if (stream)
      sf_close(stream);

    // Make sure file's in the right spot for the next thing
    file.seek(static_cast<long>(vio.origin + vio.size));
  }

  bool Sound::exists() const {
#ifndef NO_SOUND
    return !samples.empty();
#else
    return true; // don't generate errors
#endif
  }

  void Sound::destroy() {
#ifndef NO_SOUND
    samples.clear();
#endif
  }

  // Music
  Music::Music() {}

  Music::~Music() {}

  void Music::play() const {
#ifndef NO_SOUND
    music = nullptr;
    if (enabled) {
#if 0
      if (stream_intro.stream) {
        sf_close(stream_intro.stream);
        stream_intro.stream = nullptr;
      }
      if (stream_loop.stream) {
        sf_close(stream_loop.stream);
        stream_loop.stream = nullptr;
      }

      music_loop = true;
      music_end = false;

      // Open stream
      SF_INFO _info;
      // memset(&_info, 0, sizeof(_info));

      if (fileExists(intro)) {
        FILE *_fp = FOPEN(intro, "rb");
        if (_fp) {
          stream_intro.stream =
              sf_open_fd(fileno(_fp), SFM_READ, &_info, SF_TRUE);
          if (stream_intro.stream) {
            stream_intro.c_samples = _info.frames;
            stream_intro.sample_rate = _info.samplerate;
            stream_intro.channels = _info.channels;

            music_loop = false;
          }
        }
      }

      if (fileExists(loop)) {
        FILE *_fp = FOPEN(loop, "rb");
        if (_fp) {
          stream_loop.stream =
              sf_open_fd(fileno(_fp), SFM_READ, &_info, SF_TRUE);
          if (stream_loop.stream) {
            stream_loop.c_samples = _info.frames;
            stream_loop.sample_rate = _info.samplerate;
            stream_loop.channels = _info.channels;
          }
        }
      }
#endif

      music_is_loop = true;

      if (intro.exists()) {
        music_is_loop = false;
      }

      music = this;
      i_music_sample = 0.0;
    }
#else
    music = this;
#endif
  }

  void Music::stop() { music = nullptr; }

  void Music::createFromFile(const std::string &szIntro_,
                             const std::string &szLoop_) {
#ifndef NO_SOUND
    if (szIntro_.length()) {
      intro.createFromFile(szIntro_);
    }
    if (szLoop_.length()) {
      loop.createFromFile(szLoop_);
    }
#endif
  }

  bool Music::exists() const {
#ifndef NO_SOUND
    return loop.exists();
#else
    return true; // don't generate errors
#endif
  }

  bool Music::isPlaying() const { return music == this; }

  const Sound *Music::getcIntro() const { return &intro; }
  const Sound *Music::getcLoop() const { return &loop; }

  // Voice
#if 0
  Voice::Voice() {}

  Voice::~Voice() {}

  void Voice::createFromFile(const char *filename) {
    sound.createFromFile(filename);
  }

  bool Voice::exists() { return sound.exists(); }
#endif

  // Speaker
  Speaker::Speaker() {}

  Speaker::~Speaker() {}

  void Speaker::init() {
#ifndef NO_SOUND
    for (int i = 0; i < SPEAKER_SOURCE_MAX; i++) {
      if (!speaker_sources[i].speaker) {
        speaker_sources[i].speaker = this;
        break;
      }
    }
#endif
  }

  void Speaker::play(const Voice *voice) const {
#ifndef NO_SOUND
    for (int i = 0; i < SPEAKER_SOURCE_MAX; i++) {
      if (speaker_sources[i].speaker == this) {
        speaker_sources[i].sound = (Sound *)voice;
        speaker_sources[i].i_sample = 0.0;

        // float freq = 1.0f;
        // if(randomize)
        float freq = 1.0f + util::rollf() * 0.1f - 0.05f;
        speaker_sources[i].frequency = freq;
        break;
      }
    }
#endif
  }

  void Speaker::stop() const {
#ifndef NO_SOUND
    for (int i = 0; i < SPEAKER_SOURCE_MAX; i++) {
      if (speaker_sources[i].speaker == this) {
        speaker_sources[i].sound = nullptr;
      }
    }
#endif
  }
} // namespace audio
