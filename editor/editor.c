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

extern int __DEBUG;

static int _done = 0;
static Uint32 frame_delay = 33;
static float fps = 0;

GFC_Vector3D camera = { 0, 0, 0 };
GFC_Vector3D cameraPos = { 0, 50, 10 };

void parse_arguments(int argc, char* argv[]);
void game_frame_delay();

void exitGame()
{
    _done = 1;
}

int main(int argc, char* argv[])
{
    //local variables
    UIElement* healthBar, * health, * exp, * sword, * bow, * flamethrower, * bomb, * rocks, * border;
    //initializtion    
    parse_arguments(argc, argv);
    init_logger("gf3d.log", 0);
    slog("gf3d begin");
    //gfc init
    gfc_input_init("config/input.cfg");
    gfc_config_def_init();
    gfc_action_init(1024);
    //gf3d init
    gf3d_vgraphics_init("config/setup.cfg");
    gf2d_font_init("config/font.cfg");
    gf2d_actor_init(1000);

    entity_init(2000);
    enemy_init(1000);
    scene_init(32);
    gf2d_ui_init(100);

    Scene* scene_1;
    scene_1 = scene_new();
    scene_set_active(scene_1);

    //game init
    srand(SDL_GetTicks());
    slog_sync();

    UIElement* cursor;
    cursor = gf2d_ui_cursor();
    cursor->sprite = gf2d_sprite_load_image("images/player_ui/CursorWhite.png");

    UIElement* button;
    button = gf2d_ui_input_box();
    button->sprite = gf2d_sprite_load_image("images/editor/InputBox.png");
    button->position = gfc_vector2d(100, 100);
    button->key = "count";

    // main game loop    
    while (!_done)
    {
        gfc_input_update();
        gf2d_mouse_update();
        gf2d_font_update();
        //camera updaes


        entity_think_all();
        enemy_think_all();
        gf2d_ui_think_all();

        entity_update_all();
        entity_collision_check_all();

        gf3d_camera_look_at(gfc_vector3d(0,0,0), &cameraPos);

        gf3d_camera_update_view();

        gf3d_vgraphics_render_start();
            entity_draw_all();
            gf2d_ui_draw_all();
            gf2d_font_draw_line_tag(button->text, FT_H1, GFC_COLOR_BLACK, gfc_vector2d(105, 105));
            //gf2d_font_draw_line_tag(gfc_stringf("lv: %d", player->level)->buffer, FT_H1, GFC_COLOR_BLACK, gfc_vector2d(10, 115));
        gf3d_vgraphics_render_end();

        if (gfc_input_key_down("l") || gfc_input_command_down("exit"))
        {
            SDL_SetRelativeMouseMode(SDL_FALSE);
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

void parse_arguments(int argc, char* argv[])
{
    int a;

    for (a = 1; a < argc;a++)
    {
        if (strcmp(argv[a], "--debug") == 0)
        {
            __DEBUG = 1;
        }
    }
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
