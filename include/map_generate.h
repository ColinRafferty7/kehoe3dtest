#ifndef __MAP_GENERATE_H__
#define __MAP_GENERATE_H__

#include "simple_logger.h"
#include "entity.h"
#include "gf3d_model.h"
#include "enemy.h"

void map_generate_level();

void spawn_random(int row, int column, int height);

#endif