#include "audio.h"
#include "basic/memallocate.h"
#include <string.h>
#include <limits.h>
#include "core/assert.h"

#include <AL/alc.h>
#include <AL/alext.h>
#include <sndfile.h>

static char* get_al_error_msg(ALCenum err) {
    switch (err) {
      case AL_NO_ERROR: return "AL_NO_ERROR";
      case AL_INVALID_ENUM: return "AL_INVALID_ENUM";
      case AL_INVALID_VALUE: return "AL_INVALID_VALUE";
      case AL_OUT_OF_MEMORY: return "AL_OUT_OF_MEMORY";
      default:
        return "Unknown error code";
    }
}

void al_check_error() {
    ALCenum error;

    error = alGetError();
    if (error != AL_NO_ERROR) {
        LOG_ERROR(" al error %d %s\n", error, get_al_error_msg(error));
    }
}

ALuint gen_sound_buffer(const char* file_name) {
    ASSERT_MSG(file_name != NULL, "invalid audio file name");
    SF_INFO sf_info;
    SNDFILE* sndfile = sf_open(file_name, SFM_READ, &sf_info);
    if (!sndfile) {
        return 0;
    }
    if (sf_info.frames < 1 || sf_info.frames > (sf_count_t)(INT_MAX / sizeof(short)) / sf_info.channels) {
        sf_close(sndfile);
        return 0;
    }

    ALenum format = AL_NONE;
    if (sf_info.channels == 1) {
        format = AL_FORMAT_MONO16;
    }
    else if (sf_info.channels == 2) {
        format = AL_FORMAT_STEREO16;
    }
    else if (sf_command(sndfile, SFC_WAVEX_GET_AMBISONIC, NULL, 0) == SF_AMBISONIC_B_FORMAT) {
        if (sf_info.channels == 3) {
            format = AL_FORMAT_BFORMAT2D_16;
        }
        else if (sf_info.channels == 4) {
            format = AL_FORMAT_BFORMAT3D_16;
        }
    }

    if (!format) {
        return 0;
    }

    short* buf = MALLOC(sf_info.frames * sf_info.channels * sizeof(short));
    if (!buf) {
        return 0;
    }

    sf_count_t frames_count = sf_readf_short(sndfile, buf, sf_info.frames);
    if (frames_count < 1) {
        free(buf);
        return 0;
    }
    u64 size = frames_count * sf_info.channels * sizeof(short);
    ALuint buffer;
    alGenBuffers(1, &buffer);
    alBufferData(buffer, format, buf, size, sf_info.samplerate);
    FREE(buf);
    sf_close(sndfile);

    ALenum err = alGetError();
    if (err != AL_NO_ERROR) {
        alDeleteBuffers(1, &buffer);
        return 0;
    }

    return buffer;
}
static void list_audio_devices(const ALCchar *devices)
{
    const ALCchar *device = devices, *next = devices + 1;
    size_t len = 0;

    fprintf(stdout, "Devices list:\n");
    fprintf(stdout, "----------\n");
    while (device && *device != '\0' && next && *next != '\0') {
            fprintf(stdout, "%s\n", device);
            len = strlen(device);
            device += (len + 1);
            next += (len + 2);
    }
    fprintf(stdout, "----------\n");
}

void init_audio(audio_context* audio) {
    ALCdevice* device;
    device = alcOpenDevice(NULL);
    if (!device) {
        LOG_WARN(" %s", "failed to open autdio device\n");
    }

    ALboolean enumeration;

    enumeration = alcIsExtensionPresent(NULL, "ALC_ENUMERATION_EXT");
    if (enumeration == AL_FALSE)
        LOG_TRACE(" %s", "not support enumeration\n");
    else
        LOG_TRACE(" %s", "support enumeration\n");

    list_audio_devices(alcGetString(NULL, ALC_DEVICE_SPECIFIER));

    audio->context = alcCreateContext(device, NULL);
    if (!alcMakeContextCurrent(audio->context)) {
        LOG_ERROR(" %s", "failed to make context current\n");
    }
    al_check_error();
}

void shutdown_audio(audio_context* audio) {
    ALCdevice* device = alcGetContextsDevice(audio->context);
    alcMakeContextCurrent(NULL);
    alcDestroyContext(audio->context);
    alcCloseDevice(device);
}

ALuint create_audio_source(f32 pitch, f32 gain, vec3 position, vec3 velocity, i32 loop) {
    ALuint source;
    alGenSources(1, &source);

    alSourcef(source, AL_PITCH, pitch);
    al_check_error();
    alSourcef(source, AL_GAIN, gain);
    al_check_error();
    alSourcefv(source, AL_POSITION, position);
    al_check_error();
    alSourcefv(source, AL_VELOCITY, velocity);
    al_check_error();
    alSourcei(source, AL_LOOPING, loop ? AL_TRUE : AL_FALSE);
    al_check_error();
    return source;
}

// static AudioSourceState al_audio_state_map[] = {
//     [AudioSourceInital] = AL_INITIAL,
//     [AudioSourcePlaying] = AL_PLAYING,
//     [AudioSourcePaused] = AL_PAUSED,
//     [AudioSourceStopped] = AL_STOPPED,
// };

AudioSourceState get_audio_source_state(u32 source) {
    i32 source_state;
    alGetSourcei(source, AL_SOURCE_STATE, &source_state);
    if (alGetError() != AL_NO_ERROR) {
        return AudioSourceUnkwon;
    }
    return source_state - 0x1011 + 1;
}

void set_audio_listener_properties(vec3 position, vec3 velocity, ALfloat orientation[6]) {
    alListenerfv(AL_POSITION, position);
    al_check_error();
    alListenerfv(AL_VELOCITY, velocity);
    al_check_error();
    alListenerfv(AL_ORIENTATION, orientation);
    al_check_error();
}

void set_audio_source_buffer(u32 source, u32 buffer) {
    alSourcei(source, AL_BUFFER, buffer);
}

void audio_play(u32 source) {
    alSourcePlay(source);
}

void release_audio_source(u32 source) {
    alDeleteSources(1, &source);
}

void release_audio_source_buffer(u32 buffer) {
    alDeleteBuffers(1, &buffer);
}
