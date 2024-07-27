#ifndef _AUDIO_
#define _AUDIO_
#include "util.h"
#include <cglm/cglm.h>

#include <AL/al.h>
#include <AL/alc.h>

void al_check_error();
u32 gen_sound_buffer(const char* file_name);

typedef struct {
    ALCcontext* context;
} audio_context;

void init_audio(audio_context* audio);
void shutdown_audio(audio_context* audio);

u32 create_audio_source(float pitch, float gain, vec3 position, vec3 velocity, int loop);
void set_audio_listener_properties(vec3 position, vec3 velocity, f32 orientation[6]);

#endif
