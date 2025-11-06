#ifndef __COLLISION_H__
#define __COLLISION_H__

#include "entity.h"

void collision_check(Entity* entA, Entity* entB);

void collision_slope_resolve(Entity* entA, Entity* entB);

void collision_resolve(Entity* entA, Entity* entB);

void collision_enemy_resolve(Entity* entA, Entity* entB);

#endif