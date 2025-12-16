#include "map_generate.h"

#define SCALE 16
#define SIZE 8
#define HEIGHT 2
#define SLOPES 4

void map_generate_level()
{
	// TODO: Entity update order is not good
	// TODO: Optimize and restructure the code. This demonstrates horrible coding practices

	int scale = 16;
	int size = 8;
	int height = 2;
	int slopes = 4;

	int map_height[8][8];

	for (int i = (size * -1); i < size; i++)
	{
		for (int j = (size * -1); j < size; j++)
		{
			int blockHeight;
			if (i == size * -1 || i == size - 1 || j == size * -1 || j == size - 1)
			{
				blockHeight = height + 1;
			}
			else
			{
				blockHeight = gfc_random_int(height);
			}

			map_height[i][j] = blockHeight;
			
			for (int k = 0; k < blockHeight + 1; k++)
			{
				Entity* block;
				block = entity_new();
				block->isStatic = 1;
				block->name = "Grid Block";
				block = gf3d_model_load(block, "models/primitives/cube.model");

				gfc_vector3d_scale(block->scale, block->scale, scale);
				gfc_vector3d_add(block->position, block->position, gfc_vector3d((2 * scale * i), (2 * scale * j), (k * 2 * scale) - 150));

				entity_update(block);
			}
		}
	}

	for (int i = (size * -1); i < size; i++)
	{
		for (int j = (size * -1); j < size; j++)
		{
			int chance = gfc_random_int(slopes);

			if (chance == 0)
			{
				if (map_height[i + 1][j] - map_height[i - 1][j] == 1 && map_height[i][j] != height - 1)
				{
					Entity* slope;
					slope = entity_new();
					slope->isStatic = 1;
					slope->isSlope = 1;
					slope->name = "Slope Block";
					slope = gf3d_model_load(slope, "models/slope.model");

					gfc_vector3d_scale(slope->scale, slope->scale, scale);
					gfc_vector3d_add(slope->position, slope->position, gfc_vector3d((2 * scale * i), (2 * scale * j), ((map_height[i][j] + 1) * 2 * scale) - 150));
				}
			}
			if (chance == 1)
			{
				if (map_height[i - 1][j] - map_height[i + 1][j] == 1 && map_height[i][j] != height - 1)
				{
					Entity* slope;
					slope = entity_new();
					slope->isStatic = 1;
					slope->isSlope = 1;
					slope->name = "Slope Block";
					slope = gf3d_model_load(slope, "models/slope.model");

					slope->rotation.z = GFC_PI;

					gfc_vector3d_scale(slope->scale, slope->scale, scale);
					gfc_vector3d_add(slope->position, slope->position, gfc_vector3d((2 * scale * i), (2 * scale * j), ((map_height[i][j] + 1) * 2 * scale) - 150));
				}
			}
			if (chance == 1)
			{
				if (map_height[i][j + 1] - map_height[i][j - 1] == 1 && map_height[i][j] != height - 1)
				{
					Entity* slope;
					slope = entity_new();
					slope->isStatic = 1;
					slope->isSlope = 1;
					slope->name = "Slope Block";
					slope = gf3d_model_load(slope, "models/slope.model");

					slope->rotation.z = GFC_HALF_PI;

					gfc_vector3d_scale(slope->scale, slope->scale, scale);
					gfc_vector3d_add(slope->position, slope->position, gfc_vector3d((2 * scale * i), (2 * scale * j), ((map_height[i][j] + 1) * 2 * scale) - 150));
				}
			}
			if (chance == 3)
			{
				if (map_height[i][j - 1] - map_height[i][j + 1] == 1 && map_height[i][j] != height - 1)
				{
					Entity* slope;
					slope = entity_new();
					slope->isStatic = 1;
					slope->isSlope = 1;
					slope->name = "Slope Block";
					slope = gf3d_model_load(slope, "models/slope.model");

					slope->rotation.z = GFC_PI_HALFPI;

					gfc_vector3d_scale(slope->scale, slope->scale, scale);
					gfc_vector3d_add(slope->position, slope->position, gfc_vector3d((2 * scale * i), (2 * scale * j), ((map_height[i][j] + 1) * 2 * scale) - 150));
				}
			}
		}
	}

	//for (int i = (size * -1); i < size; i++)
	//{
	//	for (int j = (size * -1); j < size; j++)
	//	{
	//		if (gfc_random_int(4) == 0)
	//		{
	//			//spawn_random(i, j, map_height[i][j]);
	//		}
	//	}
	//}
}

void spawn_random(int row, int column, int height)
{
	int type = gfc_random_int(5);
	Enemy* enemy;
	enemy = enemy_new();

	if (type == 1)
	{
		//enemy_create_walker(enemy);
	}
	else if (type == 2)
	{
		//enemy_create_shooter(enemy);
	}
	else if (type == 3)
	{
		//enemy_create_jumper(enemy);
	}
	else if (type == 4)
	{
		//enemy_create_climber(enemy);
	}
	//if (!enemy->ent) return;
	//enemy->ent->position = gfc_vector3d((row * scale * 2) + gfc_random_int(64), (column * scale * 2) + gfc_random_int(64), (height * scale * 2) - 150);
}