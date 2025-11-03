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
        if (entA->velocity.z < 0)
        { 
            entA->velocity.z = 0;
        }
        
        entA->position.z += overlapZ;
    }
    entity_update(entA);
}

void collision_check(Entity* entA, Entity* entB)
{
    if (gfc_box_overlap(entA->boundingBox, entB->boundingBox) == 1)
    {
        collision_resolve(entA, entB);
    }
}
