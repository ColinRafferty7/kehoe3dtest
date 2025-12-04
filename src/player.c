#include "player.h"

Entity* player_init(Entity* player)
{
    player->name = "Dino";
    player->isPlayer = 1;
    player->persist = 1;
    player->canClimb = 1;
    player->max_health = 100;
    player->health = 100;
    player->exp_goal = 100;
    player = gf3d_model_load(player, "models/dino.model");
}