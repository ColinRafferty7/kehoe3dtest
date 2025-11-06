#include "physics.h"

void physics_update(Entity* ent)
{
    ent->velocity.z += GRAVITY * 0.033f;

    if (ent->velocity.z < -100.0f)
    {
        ent->velocity.z = -100.f;
    }

    ent->position.x += ent->velocity.x * 0.033f;
    ent->position.y += ent->velocity.y * 0.033f;
    ent->position.z += ent->velocity.z * 0.033f;
}

float physics_get_delta_time()
{
    static Uint32 lastTime = 0;
    Uint32 currentTime = SDL_GetTicks();

    float deltaTime = 0.0f;

    if (lastTime != 0)
    {
        deltaTime = (currentTime - lastTime) / 1000.0f;
    }

    lastTime = currentTime;

    return deltaTime;
}