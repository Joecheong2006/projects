#ifndef _ANIM_DURATION_SYSTEM_H_
#define _ANIM_DURATION_SYSTEM_H_
#include "anim_duration.h"

typedef struct anim_duration_system anim_duration_system;

anim_duration_system* get_anim_duration_system();

void create_anim_duration(anim_duration* duration);
void update_anim_system();
void delete_anim_duration(anim_duration* duration);
int get_anim_duration_num();

void setup_anim_system();
void shutdown_anim_system();

#endif
