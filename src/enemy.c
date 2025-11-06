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

Enemy* enemy_create_walker()
{
	Enemy* enemy;
    enemy = enemy_new();
	enemy->ent->scale = gfc_vector3d(4, 4, 10);
	enemy->ent->position = gfc_vector3d(10, 10, 0);
	enemy->ent = gf3d_model_load(enemy->ent, "models/primitives/enemy.model");
    enemy_add_think(enemy, enemy_walk);

	return enemy;
}

Enemy* enemy_create_shooter()
{
    Enemy* enemy;
    enemy = enemy_new();
    enemy->ent->scale = gfc_vector3d(4, 4, 10);
    enemy->ent->position = gfc_vector3d(10, 10, 0);
    enemy->ent = gf3d_model_load(enemy->ent, "models/primitives/enemy.model");
    enemy->approach_distance = 100;
    enemy->attackTime = 1000;
    enemy_add_think(enemy, enemy_approach);
    enemy_add_think(enemy, enemy_shoot);

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
}

void enemy_approach(Enemy* enemy)
{
    Entity* player;
    player = entity_get_player();

    GFC_Vector3D direction;
    direction = gfc_vector3d(player->position.x - enemy->ent->position.x, player->position.y - enemy->ent->position.y, 0);

    if (gfc_vector3d_magnitude(direction) <= enemy->approach_distance)
    {
        enemy->approached = 1;
        return;
    }
    else
    {
        enemy->approached = 0;
    }

    gfc_vector3d_normalize(&direction);

    enemy->ent->position.x += direction.x * 0.5f;
    enemy->ent->position.y += direction.y * 0.5f;
}

void enemy_spawn_arrow(Enemy* enemy, GFC_Vector3D direction)
{
    Entity* arrow;
    arrow = entity_new();
    arrow->isProj = 1;
    arrow->position = enemy->ent->position;
    arrow->name = "Arrow";
    arrow = gf3d_model_load(arrow, "models/primitives/arrow.model");
    arrow->scale.z = 3;
    arrow->rotation.x = GFC_HALF_PI;
    arrow->velocity.x = direction.x * 70;
    arrow->velocity.y = direction.y * 70;
    arrow->velocity.z = 15;
}

void enemy_shoot(Enemy* enemy)
{

    if (!enemy->approached) return;

    if (SDL_GetTicks() < enemy->last_attack + enemy->attackTime) return;

    Entity* player;
    player = entity_get_player();

    GFC_Vector3D direction;
    direction = gfc_vector3d(player->position.x - enemy->ent->position.x, player->position.y - enemy->ent->position.y, 0);

    gfc_vector3d_normalize(&direction);

    enemy_spawn_arrow(enemy, direction);

    enemy->last_attack = SDL_GetTicks();
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
        if (&enemy_system.enemy_list[i])
        {
            enemy_think(&enemy_system.enemy_list[i]);
        }
    }
}