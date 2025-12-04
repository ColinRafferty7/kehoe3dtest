#include "gf2d_ui.h"

#include "simple_logger.h"

typedef struct
{
	UIElement* ui_list;
	Uint32 ui_max;
}UIManager;

UIManager ui_manager = { 0 };

void gf2d_ui_draw(UIElement* ui)
{
	if (!ui->sprite) return;

	gf2d_sprite_draw_image(
		ui->sprite,
		ui->position,
		ui->scale
	);
}

void gf2d_ui_draw_all()
{
	for (int i = 0; i < ui_manager.ui_max; i++)
	{
		if (!ui_manager.ui_list[i]._inuse) continue;

		gf2d_ui_draw(&ui_manager.ui_list[i]);
	}
}

void gf2d_ui_init(Uint32 ui_max)
{
	if (!ui_max)
	{
		slog("Cannot initialize ui max of 0");
		return;
	}

	ui_manager.ui_list = gfc_allocate_array(sizeof(UIElement), ui_max);
	ui_manager.ui_max = ui_max;

	atexit(gf2d_ui_close);
}

UIElement* gf2d_ui_new()
{
	for (int i = 0; i < ui_manager.ui_max; i++)
	{
		if (ui_manager.ui_list[i]._inuse) continue;

		memset(&ui_manager.ui_list[i], 0, sizeof(UIElement));

		ui_manager.ui_list[i]._inuse = 1;
		ui_manager.ui_list[i].position = gfc_vector2d(0, 0);
		ui_manager.ui_list[i].scale = gfc_vector2d(1, 1);

		return &ui_manager.ui_list[i];
	}

	return NULL;
}

void gf2d_ui_free(UIElement* ui)
{
	if (!ui) return;
	free(ui->sprite);
	free(ui);
}

void gf2d_ui_close()
{
	if (!ui_manager.ui_list) return;

	for (int i = 0; i < ui_manager.ui_max; i++)
	{
		gf2d_ui_free(&ui_manager.ui_list[i]);
	}
	free(ui_manager.ui_list);
}