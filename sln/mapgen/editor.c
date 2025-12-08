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

void exitGame()
{
	_done = 1;
}

int main(int argc, char* argv[])
{
	init_logger("editor.log", 0);
	
	while (!_done)
	{
		slog("Editor");

		if (gfc_input_key_down("l") || gfc_input_command_down("exit"))
		{
			_done = 1; // exit condition
		}
	}
}
