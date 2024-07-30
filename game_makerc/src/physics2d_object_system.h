#ifndef _PHYSICS2D_OBJECT_SYSTEM_
#define _PHYSICS2D_OBJECT_SYSTEM_
#include "physics2d/rigid2d.h"

typedef struct physics2d_object_system physics2d_object_system;

physics2d_object_system* get_physics2d_object_system();
void create_physics2d_object(rigid2d* body);
void destory_physics2d_object(rigid2d* body);

void setup_physics2d_object_system();
void update_physics2d_object_system();
void shutdown_physics2d_object_system();

#endif
