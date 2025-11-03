#ifndef __PHYSICS_H__
#define __PHYSICS_H__

#include "simple_logger.h"
#include "entity.h"

#define GRAVITY -20.0f
#define TERMINAL_VELO -100.0f

void physics_update(Entity* ent);

float physics_get_delta_time();

#endif