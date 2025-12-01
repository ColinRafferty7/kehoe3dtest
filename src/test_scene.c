#include <SDL.h>            

#include "test_scene.h"

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

extern int __DEBUG;

static int _done = 0;
static Uint32 frame_delay = 33;
static float fps = 0;

GFC_Vector3D test_camera;

void test_exitGame()
{
    _done = 1;
}

int test_main()
{
    //local variables
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

    //game init
    srand(SDL_GetTicks());
    slog_sync();

    test_camera = gfc_vector3d(0, 50, 10);
    gf3d_camera_look_at(gfc_vector3d(0, 0, 0), &test_camera);

    Mesh* skyMesh;
    Texture* skyTexture;
    MeshUBO skyUBO;
    GFC_Matrix4 skyMat;

    skyMesh = gf3d_mesh_load_obj("models/sky/sky.obj");
    skyTexture = gf3d_texture_load("models/sky/sky.png");
    gfc_matrix4_identity(skyMat);
    skyUBO = gf3d_mesh_get_ubo(skyMat, GFC_COLOR_WHITE);

    Entity* transparentTest;
    transparentTest = entity_new();
    transparentTest = gf3d_model_load(transparentTest, "models/dino.model");
    transparentTest->isStatic = 1;

    // main game loop    
    while (!_done)
    {
        gfc_input_update();
        gf2d_mouse_update();
        gf2d_font_update();
        //camera updaes

        entity_think_all();
        enemy_think_all();

        entity_update_all();
        entity_collision_check_all();

        gf3d_camera_update_view();

        gf3d_vgraphics_render_start();
        //2D draws

            gf3d_mesh_queue_render(skyMesh, gf3d_mesh_get_sky_pipeline(), &skyUBO, skyTexture);
            entity_draw_all();

        gf3d_vgraphics_render_end();

        if (gfc_input_key_down("l") || gfc_input_command_down("exit"))
        {
            SDL_SetRelativeMouseMode(SDL_FALSE);
            _done = 1; // exit condition
        }
        test_frame_delay();
    }
    vkDeviceWaitIdle(gf3d_vgraphics_get_default_logical_device());
    //cleanup
    slog("gf3d program end");
    exit(0);
    slog_sync();
    return 0;
}

void test_frame_delay()
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
