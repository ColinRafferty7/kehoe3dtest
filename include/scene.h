#ifndef __SCENE_H__
#define __SCENE_H__

#include "entity.h"

typedef struct
{
	Entity** entities;
	Uint32 ent_count;
	Uint32 ent_max;

	Uint8 _inuse;
}Scene;

void scene_init(Uint32 scene_max);

Scene* scene_new();

void scene_free(Scene* scene);

void scene_close();

void scene_add_entity(Scene* scene, Entity* ent);

void scene_set_active(Scene* scene);

Scene* scene_get_active();

void scene_main_menu();

#endif