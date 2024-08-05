#ifndef _AUDIO_H_
#define _AUDIO_H_
#include "core/defines.h"
#include <cglm/vec3.h>

void al_check_error();
u32 gen_sound_buffer(const char* file_name);

typedef struct ALCcontext ALCcontext;
typedef struct {
    ALCcontext* context;
} audio_context;

void init_audio(audio_context* audio);
void shutdown_audio(audio_context* audio);

u32 create_audio_source(f32 pitch, f32 gain, vec3 position, vec3 velocity, i32 loop);

typedef enum {
    AudioSourceInital,
    AudioSourcePlaying,
    AudioSourcePaused,
    AudioSourceStopped
} AudioSourceState;

AudioSourceState get_audio_source_state(u32 source);
void set_audio_listener_properties(vec3 position, vec3 velocity, f32 orientation[6]);
void set_audio_source_buffer(u32 source, u32 buffer);
void audio_play(u32 source);
void release_audio_source(u32 source);
void release_audio_source_buffer(u32 buffer);

#endif
