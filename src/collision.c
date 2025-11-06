#include "simple_logger.h"
#include "collision.h"
#include "entity.h"

void collision_resolve(Entity* entA, Entity* entB)
{
    GFC_Box boxA = entA->boundingBox;
    GFC_Box boxB = entB->boundingBox;

    float a_min_x = fminf(boxA.x, boxA.x + boxA.w);
    float a_max_x = fmaxf(boxA.x, boxA.x + boxA.w);
    float a_min_y = fminf(boxA.y, boxA.y + boxA.h);
    float a_max_y = fmaxf(boxA.y, boxA.y + boxA.h);
    float a_min_z = fminf(boxA.z, boxA.z + boxA.d);
    float a_max_z = fmaxf(boxA.z, boxA.z + boxA.d);

    float b_min_x = fminf(boxB.x, boxB.x + boxB.w);
    float b_max_x = fmaxf(boxB.x, boxB.x + boxB.w);
    float b_min_y = fminf(boxB.y, boxB.y + boxB.h);
    float b_max_y = fmaxf(boxB.y, boxB.y + boxB.h);
    float b_min_z = fminf(boxB.z, boxB.z + boxB.d);
    float b_max_z = fmaxf(boxB.z, boxB.z + boxB.d);

    float moveX1 = b_max_x - a_min_x;  
    float moveX2 = a_max_x - b_min_x;  
    float overlapX = (moveX1 < moveX2 ? moveX1 : -moveX2);

    float moveY1 = b_max_y - a_min_y;
    float moveY2 = a_max_y - b_min_y;
    float overlapY = (moveY1 < moveY2 ? moveY1 : -moveY2);

    float moveZ1 = b_max_z - a_min_z;
    float moveZ2 = a_max_z - b_min_z;
    float overlapZ = (moveZ1 < moveZ2 ? moveZ1 : -moveZ2);

    float absX = fabsf(overlapX);
    float absY = fabsf(overlapY);
    float absZ = fabsf(overlapZ);

    if (absX < absY && absX < absZ)
    {
        entA->position.x += overlapX;
    }
    else if (absY < absZ)
    {
        entA->position.y += overlapY;
    }
    else
    {
        entA->velocity.x *= 0.5f;
        entA->velocity.y *= 0.5f;
        entA->velocity.z = 0;
        
        entA->position.z += overlapZ;
    }
    entity_update(entA);
}

float collision_corner_rotate_x(Entity* ent)
{
    float newPoint;

    newPoint = ((sin(ent->rotation.z - (GFC_HALF_PI / 2))) * (GFC_ROOT2 / 2)) + 0.5f;

    return ent->boundingBox.x + (newPoint * ent->boundingBox.w);
}

float collision_corner_rotate_y(Entity* ent)
{
    float newPoint;

    newPoint = ((sin(ent->rotation.z - (GFC_PI_HALFPI / 2))) * (GFC_ROOT2 / 2)) + 0.5f;

    return ent->boundingBox.y + (newPoint * ent->boundingBox.h);
}

void collision_slope_resolve(Entity* entA, Entity* entB)
{
    GFC_Vector3D entB_Corner = gfc_vector3d(
        collision_corner_rotate_x(entB),
        collision_corner_rotate_y(entB),
        entB->boundingBox.z);

    float final_z_pos;

    if (fabsf(entB->rotation.z) < GFC_EPSILON)
    {
        final_z_pos = entB->boundingBox.z + (entA->position.x - entB_Corner.x);
        Uint8 box;
        if (entB_Corner.y > entA->position.y && final_z_pos - entA->boundingBox.z > 2.0f) box = 1;

        if (entB_Corner.y + entB->boundingBox.h < entA->position.y && final_z_pos - entA->boundingBox.z > 2.0f) box = 1;

        if (entB_Corner.x + entB->boundingBox.w < entA->position.x) box = 1;

        if (box)
        {
            collision_resolve(entA, entB);
            return;
        }
    }
    else if (fabsf(entB->rotation.z - GFC_HALF_PI) < GFC_EPSILON)
    {
        final_z_pos = entB->boundingBox.z + (entA->position.y - entB_Corner.y);
        Uint8 box;
        if (entB_Corner.y + entB->boundingBox.h < entA->position.y) box = 1;

        if (entB_Corner.x < entA->position.x && final_z_pos - entA->boundingBox.z > 2.0f) box = 1;

        if (entB_Corner.x - entB->boundingBox.w > entA->position.x && final_z_pos - entA->boundingBox.z > 2.0f) box = 1;

        if (box)
        {
            collision_resolve(entA, entB);
            return;
        }
    }
    else if (fabsf(entB->rotation.z - GFC_PI) < GFC_EPSILON)
    {
        final_z_pos = entB->boundingBox.z - (entA->position.x - entB_Corner.x);
        Uint8 box;
        if (entB_Corner.y < entA->position.y && final_z_pos - entA->boundingBox.z > 2.0f) box = 1;

        if (entB_Corner.y - entB->boundingBox.h > entA->position.y && final_z_pos - entA->boundingBox.z > 2.0f) box = 1;

        if (entB_Corner.x - entB->boundingBox.w > entA->position.x) box = 1;

        if (box)
        {
            collision_resolve(entA, entB);
            return;
        }
    }
    else if (fabsf(entB->rotation.z - GFC_PI_HALFPI) < GFC_EPSILON)
    {
        final_z_pos = entB->boundingBox.z - (entA->position.y - entB_Corner.y);
        Uint8 box;
        if (entB_Corner.y - entB->boundingBox.h > entA->position.y) box = 1;

        if (entB_Corner.x > entA->position.x && final_z_pos - entA->boundingBox.z > 2.0f) box = 1;

        if (entB_Corner.x + entB->boundingBox.w < entA->position.x && final_z_pos - entA->boundingBox.z > 2.0f) box = 1;

        if (box)
        {
            collision_resolve(entA, entB);
            return;
        }
    }
    else
    {
        slog("Slopes cannot be rotated in this direction");
        return;
    }

    

    if (final_z_pos < entB->boundingBox.z || final_z_pos > (entB->boundingBox.z + entB->boundingBox.d)) return;

    if (final_z_pos < entA->boundingBox.z) return;

    if (entB_Corner.y > entA->position.y && final_z_pos - entA->boundingBox.z > 2.0f)
    {
        collision_resolve(entA, entB);
        return;
    }

    if (entB_Corner.y + entB->boundingBox.h < entA->position.y && final_z_pos - entA->boundingBox.z > 2.0f)
    {
        collision_resolve(entA, entB);
        return;
    }

    if (entB_Corner.x > entA->position.x && final_z_pos - entA->boundingBox.z > 2.0f)
    {
        collision_resolve(entA, entB);
        return;
    }

    if (entB_Corner.x + entB->boundingBox.w < entA->position.x && final_z_pos - entA->boundingBox.z > 2.0f)
    {
        collision_resolve(entA, entB);
        return;
    }

    // TODO: Solve walking onto a platform at the top of a slope

    entA->position.z = final_z_pos + (entA->boundingBox.d / 2.0f);

    entA->velocity.z = 0;
    entity_update(entA);
}




void collision_check(Entity* entA, Entity* entB)
{
    if (gfc_box_overlap(entA->boundingBox, entB->boundingBox) == 1)
    {
        if (entB->isSlope)
        {
            collision_slope_resolve(entA, entB);
        }
        else if (entA->isPlayer && entB->isEnemy)
        {
            collision_enemy_resolve(entA, entB);
        }
        else if (entA->isProj || entB->isProj)
        {
            collision_projectile_resolve(entA, entB);
        }
        else
        {
            collision_resolve(entA, entB);
        }
    }
}

void collision_enemy_resolve(Entity* entA, Entity* entB)
{
    entA->health -= 5;
    entA->velocity.x += (entA->position.x - entB->position.x) * 5;
    entA->velocity.y += (entA->position.y - entB->position.y) * 5;
    entA->position.z += 0.1f;
    entA->velocity.z += 5;
}

void collision_projectile_resolve(Entity* entA, Entity* entB)
{
    if (entB->isStatic)
    {
        entity_free(entA);
    }
    if (entB->isPlayer)
    {
        entB->health -= 5;
        entB->velocity.x += (entB->position.x - entA->position.x) * 5;
        entB->velocity.y += (entB->position.y - entA->position.y) * 5;
        entB->position.z += 0.1f;
        entB->velocity.z += 5;

        entity_free(entA);
    }      
}