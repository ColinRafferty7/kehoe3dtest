#include <SDL.h>            

#include "simple_json.h"
#include "simple_logger.h"
#include "simple_json_object.h"
#include "simple_json_value.h"

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

void editor_think()
{
    if (gf2d_mouse_button_held(1))
    {
        GFC_Vector3D direction;

        direction = cameraPos;
        direction.z = 0;
        gfc_vector3d_normalize(&direction);
        gfc_vector3d_rotate_about_z(&direction, GFC_PI);

        if (gfc_input_key_down("w"))
        {
            gfc_vector3d_add(camera, camera, direction);
            gfc_vector3d_add(cameraPos, cameraPos, direction);
        }
        if (gfc_input_key_down("s"))
        {
            gfc_vector3d_rotate_about_z(&direction, GFC_PI);
            gfc_vector3d_add(camera, camera, direction);
            gfc_vector3d_add(cameraPos, cameraPos, direction);
        }
        if (gfc_input_key_down("a"))
        {
            gfc_vector3d_rotate_about_z(&direction, GFC_HALF_PI);
            gfc_vector3d_add(camera, camera, direction);
            gfc_vector3d_add(cameraPos, cameraPos, direction);
        }
        if (gfc_input_key_down("d"))
        {
            gfc_vector3d_rotate_about_z(&direction, -1 * GFC_HALF_PI);
            gfc_vector3d_add(camera, camera, direction);
            gfc_vector3d_add(cameraPos, cameraPos, direction);
        }
    }
}

void regenerate()
{
    Scene* new_scene;
    new_scene = scene_new();
    scene_set_active(new_scene);
    camera = gfc_vector3d(0, 0, 0);
    map_generate_level();
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

    SJson* def = sj_load("def/test.def");

    int count = sj_list_get_count(def->v.array);

    for (int i = 0; i < count; i++)
    {
        SJPair* pair = (SJPair*) sj_list_get_nth(def->v.array, i);

        SJString* string = sj_value_to_json_string(pair->value);

        UIElement* inputBox;
        inputBox = gf2d_ui_input_box();
        inputBox->sprite = gf2d_sprite_load_image("images/editor/InputBox.png");
        inputBox->position = gfc_vector2d(1000, 100 * i);
        inputBox->key = pair->key->text;
        strcpy(inputBox->text, string->text);
    }

    map_generate_level();

    UIElement* cursor;
    cursor = gf2d_ui_cursor();
    cursor->sprite = gf2d_sprite_load_image("images/player_ui/CursorWhite.png");

    UIElement* button;
    button = gf2d_ui_button();
    button->sprite = gf2d_sprite_load_image("images/player_ui/Border.png");
    button->position = gfc_vector2d(550, 650);
    button->scale.x = 2;
    button->scale.y = 0.6;
    button->click = regenerate;

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

        editor_think();

        gf3d_camera_look_at(camera, &cameraPos);

        gf3d_camera_update_view();

        gf3d_vgraphics_render_start();
            entity_draw_all();
            gf2d_ui_draw_all();
            gf2d_ui_draw(cursor);
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
