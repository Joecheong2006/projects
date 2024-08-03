#ifndef _AUDIO_
#define _AUDIO_
#include "basic/util.h"
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
void set_audio_listener_properties(vec3 position, vec3 velocity, f32 orientation[6]);

#endif
