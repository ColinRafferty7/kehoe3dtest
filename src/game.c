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
#include "enemy.h";

extern int __DEBUG;

static int _done = 0;
static Uint32 frame_delay = 33;
static float fps = 0;

GFC_Vector3D camera = { 0, 0, 0 };
GFC_Vector3D cameraPos = { 0, 50, 10 };

void parse_arguments(int argc,char *argv[]);
void game_frame_delay();

void exitGame()
{
    _done = 1;
}

void dino_die(Entity* dino)
{

}

void dino_think(Entity* dino)
{
    GFC_Vector3D direction;

    direction = cameraPos;
    gfc_vector3d_normalize(&direction);
    direction.z = 0;
    gfc_vector3d_rotate_about_z(&direction, GFC_PI);

    if (gfc_input_key_down("w"))
    {
        gfc_vector3d_add(dino->position, dino->position, direction);
    }
    if (gfc_input_key_down("s"))
    {
        gfc_vector3d_rotate_about_z(&direction, GFC_PI);
        gfc_vector3d_add(dino->position, dino->position, direction);
    }    
    if (gfc_input_key_down("a"))
    {
        gfc_vector3d_rotate_about_z(&direction, GFC_HALF_PI);
        gfc_vector3d_add(dino->position, dino->position, direction);
    }
    if (gfc_input_key_down("d"))
    {
        gfc_vector3d_rotate_about_z(&direction, -1 * GFC_HALF_PI);
        gfc_vector3d_add(dino->position, dino->position, direction);
    }
    if (gfc_input_key_pressed("c"))
    {
        dino->position.z += 0.01f;
        dino->velocity.z = 25;
    }
    if (gf2d_mouse_moved())
    {
        // TODO: Add vertical camera rotation by changing camera to be an entity
        gfc_vector3d_rotate_about_z(&cameraPos, -0.01 * gf2d_mouse_get_movement().x);
        dino->rotation.z += -0.01 * gf2d_mouse_get_movement().x;
    }

    if (gfc_input_key_pressed("p"))
    {
        SDL_SetRelativeMouseMode(SDL_TRUE);
    }
    if (gfc_input_key_pressed("o"))
    {
        SDL_SetRelativeMouseMode(SDL_FALSE);
    }   
    if (gfc_input_key_pressed("e"))
    {
        Enemy* enemy;
        enemy = enemy_create();
    }   

    if (dino->health <= 0)
    {
        dino_die(dino);
    }
}

int main(int argc,char *argv[])
{
    //local variables
    Sprite *healthBar, *health;
    //initializtion    
    parse_arguments(argc,argv);
    init_logger("gf3d.log",0);
    slog("gf3d begin");
    //gfc init
    gfc_input_init("config/input.cfg");
    gfc_config_def_init();
    gfc_action_init(1024);
    //gf3d init
    gf3d_vgraphics_init("config/setup.cfg");
    gf2d_font_init("config/font.cfg");
    gf2d_actor_init(1000);

    entity_init(1024);
    enemy_init(100);

    SDL_SetRelativeMouseMode(SDL_TRUE);
    
    //game init
    srand(SDL_GetTicks());
    slog_sync();
    healthBar = gf2d_sprite_load_image("images/player_ui/HealthBar.png");
    health = gf2d_sprite_load_image("images/player_ui/Health.png");

    map_generate_level();

    Mesh* skyMesh;
    Texture* skyTexture;
    MeshUBO skyUBO;
    GFC_Matrix4 skyMat;

    skyMesh = gf3d_mesh_load_obj("models/sky/sky.obj");
    skyTexture = gf3d_texture_load("models/sky/sky.png");
    gfc_matrix4_identity(skyMat);
    skyUBO = gf3d_mesh_get_ubo(skyMat, GFC_COLOR_WHITE);

    Entity* dino;
    dino = entity_new();
    dino->name = "Dino";
    dino->isPlayer = 1;
    dino->think = dino_think;
    dino->max_health = 100;
    dino->health = 100;
    dino = gf3d_model_load(dino, "models/dino.model");

    // main game loop    
    while(!_done)
    {
        gfc_input_update();
        gf2d_mouse_update();
        gf2d_font_update();
        //camera updaes

        entity_think_all();
        enemy_think_all();
        
        entity_update_all();
        entity_collision_check_all();

        camera = gfc_vector3d_added(gfc_vector3d(dino->modelMat[3][0], dino->modelMat[3][1], dino->modelMat[3][2]), cameraPos);
        gf3d_camera_look_at(gfc_vector3d(dino->modelMat[3][0], dino->modelMat[3][1], dino->modelMat[3][2]), &camera);

        gf3d_camera_update_view();

        gf3d_vgraphics_render_start();
                //2D draws
                gf3d_mesh_queue_render(skyMesh, gf3d_mesh_get_sky_pipeline(), &skyUBO, skyTexture);
                entity_draw_all();

                gf2d_sprite_draw_image(healthBar, gfc_vector2d(10, 10), gfc_vector2d(1, 1));
                gf2d_sprite_draw_image(health, gfc_vector2d(13, 13), gfc_vector2d(dino->health / dino->max_health, 1));
                
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

void parse_arguments(int argc,char *argv[])
{
    int a;

    for (a = 1; a < argc;a++)
    {
        if (strcmp(argv[a],"--debug") == 0)
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
    fps = 1000.0/MAX(SDL_GetTicks() - then,0.001);
//     slog("fps: %f",fps);
}


/*eol@eof*/
