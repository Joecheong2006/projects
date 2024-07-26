#ifndef _SOUND_
#define _SOUND_

#include <AL/al.h>
#include <AL/alc.h>
#include <AL/alext.h>
#include <sndfile.h>

void al_check_error();

ALuint gen_sound_buffer(const char* file_name);

#endif
