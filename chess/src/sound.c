#include "sound.h"
#include "util.h"
// #include <stdlib.h>
#include "memallocate.h"
#include <limits.h>

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
        printf("al error %d %s\n", error, get_al_error_msg(error));
    }
}

ALuint gen_sound_buffer(const char* file_name) {
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

