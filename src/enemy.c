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
        enemy_system.enemy_list[i].ent->isEnemy = 1;

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
    enemy_add_think(enemy, enemy_walk);

	return enemy;
}

void enemy_add_think(Enemy* enemy, Think think)
{
    if (enemy->think_count < 16)
    {
        enemy->think_list[enemy->think_count] = think;
        enemy->think_count++;
    }
    else
    {
        slog("Enemy has too many think functions");
    }
}

void enemy_walk(Enemy* enemy)
{
    Entity* player;
    player = entity_get_player();

    GFC_Vector3D direction;
    direction = gfc_vector3d(player->position.x - enemy->ent->position.x, player->position.y - enemy->ent->position.y, 0);

    gfc_vector3d_normalize(&direction);

    enemy->ent->position.x += direction.x * 0.5f;
    enemy->ent->position.y += direction.y * 0.5f;

    slog("Enemy walking");
}

void enemy_think(Enemy* enemy)
{
    for (int i = 0; i < enemy->think_count; i++)
    {
        enemy->think_list[i](enemy);
    }
}

void enemy_think_all()
{
    for (int i = 0; i < enemy_system.enemy_max; i++)
    {
        slog("Enemy think all iteration");
        if (&enemy_system.enemy_list[i])
        {
            slog("Enemy think call");
            enemy_think(&enemy_system.enemy_list[i]);
        }
    }
}