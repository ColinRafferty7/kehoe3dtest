#include <SDL.h>            

#include "simple_json.h"
#include "simple_logger.h"

#include "gfc_input.h"
#include "gfc_config_def.h"
#include "gfc_vector.h"
#include "gfc_matrix.h"
#include "gfc_audio.h"
#include "gfc_string.h"
#include "gfc_actions.h"

#include "gf2d_sprite.h"
#include "gf2d_font.h"
#include "gf2d_actor.h"
#include "gf2d_mouse.h"

#include "gf3d_vgraphics.h"
#include "gf3d_pipeline.h"
#include "gf3d_swapchain.h"
#include "gf3d_mesh.h"
#include "entity.h"
#include "gf3d_model.h"
#include "gf3d_camera.h"
#include "map_generate.h"
#include "enemy.h"
#include "player.h"
#include "gf3d_billboard.h"
#include "gf2d_ui.h"

#include "test_scene.h"
#include "scene.h"

static int _done = 0;
static Uint32 frame_delay = 33;
static float fps = 0;

void parse_arguments(int argc, char* argv[]);
void game_frame_delay();

void exitGame()
{
	_done = 1;
}

int main(int argc, char* argv[])
{
	init_logger("editor.log", 0);
	gfc_input_init("config/input.cfg");
	gfc_config_def_init();
	gfc_action_init(1024);

	gf3d_vgraphics_init("config/setup.cfg");
	gf2d_font_init("config/font.cfg");
	gf2d_actor_init(1000);
	
	SJson* def = sj_load("def/test.def");

	int count = 0;
	int count2 = 0;
	sj_object_get_value_as_int(def, "count", &count);
	sj_object_get_value_as_int(def, "count_two", &count2);
	count++;
	count2++;
	sj_object_delete_key(def, "count");
	sj_object_delete_key(def, "count_two");
	sj_object_insert(def, "count", sj_new_int(count));
	sj_object_insert(def, "count_two", sj_new_int(count2));

	sj_save(def, "def/test.def");
	
	while (!_done)
	{
		gfc_input_update();

		if (gfc_input_key_down("l") || gfc_input_command_down("exit"))
		{
			_done = 1; // exit condition
		}

		game_frame_delay();
	}

	vkDeviceWaitIdle(gf3d_vgraphics_get_default_logical_device());
	//cleanup
	slog("gf3d program end");
	exit(0);
	slog_sync();
	return 0;
}

void game_frame_delay()
{
	Uint32 diff;
	static Uint32 now;
	static Uint32 then;
	then = now;
	slog_sync();// make sure logs get written when we have time to write it
	now = SDL_GetTicks();
	diff = (now - then);
	if (diff < frame_delay)
	{
		SDL_Delay(frame_delay - diff);
	}
	fps = 1000.0 / MAX(SDL_GetTicks() - then, 0.001);
	//     slog("fps: %f",fps);
}

/*eol@eof*/
