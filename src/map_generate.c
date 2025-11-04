#include "map_generate.h"

#define SCALE 32
#define SIZE 8
#define HEIGHT 3
#define SLOPES 4

void map_generate_level()
{
	// TODO: Entity update order is not good
	// TODO: Optimize and restructure the code. This demonstrates horrible coding practices

	int map_height[SIZE * 2][SIZE * 2];

	for (int i = (SIZE * -1); i < SIZE; i++)
	{
		for (int j = (SIZE * -1); j < SIZE; j++)
		{
			int blockHeight = gfc_random_int(HEIGHT);

			map_height[i][j] = blockHeight;
			
			for (int k = 0; k < blockHeight + 1; k++)
			{
				Entity* block;
				block = entity_new();
				block->isStatic = 1;
				block->name = "Grid Block";
				block = gf3d_model_load(block, "models/primitives/cube.model");

				gfc_vector3d_scale(block->scale, block->scale, SCALE);
				gfc_vector3d_add(block->position, block->position, gfc_vector3d((2 * SCALE * i), (2 * SCALE * j), (k * 2 * SCALE) - 150));

				entity_update(block);
			}
		}
	}

	for (int i = (SIZE * -1); i < SIZE; i++)
	{
		for (int j = (SIZE * -1); j < SIZE; j++)
		{
			int chance = gfc_random_int(SLOPES);

			if (chance == 0)
			{
				if (map_height[i + 1][j] - map_height[i - 1][j] == 1 && map_height[i][j] != HEIGHT - 1)
				{
					Entity* slope;
					slope = entity_new();
					slope->isStatic = 1;
					slope->isSlope = 1;
					slope->name = "Slope Block";
					slope = gf3d_model_load(slope, "models/slope.model");

					gfc_vector3d_scale(slope->scale, slope->scale, SCALE);
					gfc_vector3d_add(slope->position, slope->position, gfc_vector3d((2 * SCALE * i), (2 * SCALE * j), ((map_height[i][j] + 1) * 2 * SCALE) - 150));
				}
			}
			if (chance == 1)
			{
				if (map_height[i - 1][j] - map_height[i + 1][j] == 1 && map_height[i][j] != HEIGHT - 1)
				{
					Entity* slope;
					slope = entity_new();
					slope->isStatic = 1;
					slope->isSlope = 1;
					slope->name = "Slope Block";
					slope = gf3d_model_load(slope, "models/slope.model");

					slope->rotation.z = GFC_PI;

					gfc_vector3d_scale(slope->scale, slope->scale, SCALE);
					gfc_vector3d_add(slope->position, slope->position, gfc_vector3d((2 * SCALE * i), (2 * SCALE * j), ((map_height[i][j] + 1) * 2 * SCALE) - 150));
				}
			}
			if (chance == 1)
			{
				if (map_height[i][j + 1] - map_height[i][j - 1] == 1 && map_height[i][j] != HEIGHT - 1)
				{
					Entity* slope;
					slope = entity_new();
					slope->isStatic = 1;
					slope->isSlope = 1;
					slope->name = "Slope Block";
					slope = gf3d_model_load(slope, "models/slope.model");

					slope->rotation.z = GFC_HALF_PI;

					gfc_vector3d_scale(slope->scale, slope->scale, SCALE);
					gfc_vector3d_add(slope->position, slope->position, gfc_vector3d((2 * SCALE * i), (2 * SCALE * j), ((map_height[i][j] + 1) * 2 * SCALE) - 150));
				}
			}
			if (chance == 3)
			{
				if (map_height[i][j - 1] - map_height[i][j + 1] == 1 && map_height[i][j] != HEIGHT - 1)
				{
					Entity* slope;
					slope = entity_new();
					slope->isStatic = 1;
					slope->isSlope = 1;
					slope->name = "Slope Block";
					slope = gf3d_model_load(slope, "models/slope.model");

					slope->rotation.z = GFC_PI_HALFPI;

					gfc_vector3d_scale(slope->scale, slope->scale, SCALE);
					gfc_vector3d_add(slope->position, slope->position, gfc_vector3d((2 * SCALE * i), (2 * SCALE * j), ((map_height[i][j] + 1) * 2 * SCALE) - 150));
				}
			}
		}
	}
}