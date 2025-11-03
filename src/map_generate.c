#include "map_generate.h"

void map_generate_level()
{
	// TODO: Entity update order is not good

	int scale = 32;
	int size = 8;
	int height = 1;

	for (int i = (size * -1); i < size; i++)
	{
		for (int j = (size * -1); j < size; j++)
		{
			Entity* block;
			block = entity_new();
			block->isStatic = 1;
			block->name = "Grid Block";
			block = gf3d_model_load(block, "models/primitives/cube.model");
			
			gfc_vector3d_scale(block->scale, block->scale, scale);
			gfc_vector3d_add(block->position, block->position, gfc_vector3d((2 * scale * i), (2 * scale * j), (gfc_random_int(height) * 2 * scale) - 150));

			entity_update(block);
		}
	}
}