#ifndef __ENEMY_H__
#define __ENEMY_H__

#include "simple_logger.h"
#include "entity.h"
#include "gf3d_model.h"

struct Enemy;

typedef void (*Think)(struct Enemy* self);

typedef struct
{
	Uint8 _inuse;

	Entity* ent;

	Think think_list[16];
	int think_count;

	Uint8 approached;
	int approach_distance;
	Uint32 last_attack;
	Uint32 attackTime;
}Enemy;

void entity_close();

void entity_init(Uint32 entity_max);

Enemy* enemy_create_walker();

Enemy* enemy_create_shooter();

void enemy_walk(Enemy* enemy);

void enemy_approach(Enemy* enemy);

void enemy_shoot(Enemy* enemy);

void enemy_add_think(Enemy* enemy, Think think);

void enemy_think_all();

#endif