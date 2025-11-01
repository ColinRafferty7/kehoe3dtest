#include "simple_logger.h"
#include "collision.h"

void collision_check(Entity* entA, Entity* entB)
{
	if (gfc_box_overlap(entA->boundingBox, entB->boundingBox) == 1)
	{
		slog("Collision Detected Between %s and %s", entA->name, entB->name);
	}
}