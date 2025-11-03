#include "map_generate.h"

void map_generate_level()
{
	for (int i = 1; i < 9; i++)
	{
		for (int j = 1; j < 9; j++)
		{
			Entity* block;
			block = entity_new();
			block->isStatic = 1;
			block->name = "Grid Block";
			block = gf3d_model_load(block, "models/primitives/cube.model");

			gfc_vector3d_add(block->position, block->position, gfc_vector3d((2 * i) - 30, (2 * j) - 30, -95));

			entity_update(block);

			slog("(%f, %f, %f) - (%f, %f, %f)",
				block->boundingBox.x,
				block->boundingBox.y,
				block->boundingBox.z,
				block->boundingBox.w,
				block->boundingBox.h,
				block->boundingBox.d);
		}
	}
}