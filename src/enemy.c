#include "enemy.h"

typedef struct
{
    Enemy* enemy_list;
    Uint32 enemy_max;
}EnemySystem;

static EnemySystem enemy_system = { 0 };

void enemy_close()
{
    if (!enemy_system.enemy_list) return;

    for (int i = 0; i < enemy_system.enemy_max; i++)
    {
        free(&enemy_system.enemy_list[i]);
    }
    free(enemy_system.enemy_list);
}

void enemy_init(Uint32 enemy_max)
{
    if (!enemy_max)
    {
        slog("Cannot initialize enemy max of 0");
        return;
    }

    enemy_system.enemy_list = gfc_allocate_array(sizeof(Enemy), enemy_max);
    enemy_system.enemy_max = enemy_max;

    atexit(enemy_close);
}

Entity* enemy_new()
{
    for (int i = 0; i < enemy_system.enemy_max; i++)
    {
        if (enemy_system.enemy_list[i]._inuse) continue;

        memset(&enemy_system.enemy_list[i], 0, sizeof(Enemy));

        enemy_system.enemy_list[i]._inuse = 1;
        enemy_system.enemy_list[i].ent = entity_new();
        enemy_system.enemy_list[i].ent->name = "Enemy";

        return &enemy_system.enemy_list[i];
    }

    return NULL;
}

Enemy* enemy_create()
{
	Enemy* enemy;
    enemy = enemy_new();
	enemy->ent->scale = gfc_vector3d(4, 4, 10);
	enemy->ent->position = gfc_vector3d(10, 10, 0);
	enemy->ent = gf3d_model_load(enemy->ent, "models/primitives/enemy.model");

	return enemy;
}