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
}Enemy;

void entity_close();

void entity_init(Uint32 entity_max);

Enemy* enemy_create();

#endif