#include "gf2d_ui.h"

#include "simple_logger.h"
#include "gf2d_mouse.h"

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

void gf2d_ui_think(UIElement* ui)
{
	ui->think(ui);
}

void  gf2d_ui_think_all()
{
	for (int i = 0; i < ui_manager.ui_max; i++)
	{
		if (!ui_manager.ui_list[i].think) continue;

		gf2d_ui_think(&ui_manager.ui_list[i]);
	}
}

void button_think(UIElement* button)
{
	GFC_Vector2D mousePos = gf2d_mouse_get_position();
	if (gf2d_mouse_button_pressed(0))
	{
		int overlapX, overlapY;
		overlapX = mousePos.x - button->position.x;
		overlapY = mousePos.y - button->position.y;

		if (overlapX > 0 && overlapX < button->sprite->frameWidth * button->scale.x &&
			overlapY > 0 && overlapY < button->sprite->frameHeight * button->scale.y)
		{
			button->click(button);
		}
		
	}
}

void cursor_think(UIElement* cursor)
{
	GFC_Vector2D mousePos = gf2d_mouse_get_position();
	cursor->position = gfc_vector2d(mousePos.x - cursor->sprite->frameWidth * cursor->scale.x * 0.5f, mousePos.y - cursor->sprite->frameHeight * cursor->scale.y * 0.5f);
}

UIElement* gf2d_ui_cursor()
{
	UIElement* cursor;
	cursor = gf2d_ui_new();
	cursor->think = cursor_think;
}

UIElement* gf2d_ui_button()
{
	UIElement* button;
	button = gf2d_ui_new();
	button->think = button_think;
}