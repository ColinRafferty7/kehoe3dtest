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

void player_die(Entity* player)
{
    player->position = gfc_vector3d(0, 0, 0);
    player->health = 100;
}

void sword_think(Entity* sword)
{
    sword->rotation.y += 0.2;

    if (sword->rotation.y > 3)
    {
        entity_free(sword);
    }
}

void player_sword(Entity* player)
{
    GFC_Vector3D direction;

    direction = cameraPos;
    gfc_vector3d_normalize(&direction);
    direction.z = 0;
    gfc_vector3d_rotate_about_z(&direction, GFC_HALF_PI);

    Entity* sword;
    sword = entity_new();
    sword = gf3d_model_load(sword, "models/primitives/cube.model");
    sword->isAttack = 1;
    sword->isProj = 1;
    sword->scale.x = 0.2f + (player->level * 0.04f);
    sword->scale.x = 1 + (player->level * 0.2f);
    sword->scale.z = 5 + (player->level);
    sword->position = player->position;
    sword->position.x += (direction.x) * 3;
    sword->position.y += (direction.y) * 3;
    sword->position.z += 1 + (player->level * 0.2f);
    sword->rotation.z = player->rotation.z;
    sword->think = sword_think;

    gfc_vector3d_rotate_about_z(&direction, GFC_HALF_PI);

    Entity* arrow;
    arrow = entity_new();
    arrow->isProj = 1;
    arrow->isAttack = 1;
    arrow->damage = 10 + (player->level * 2);
    arrow = gf3d_model_load(arrow, "models/primitives/sword.model");
    arrow->scale.x = 5 + (player->level);
    arrow->scale.y = 5 + (player->level);
    arrow->scale.z = 3;
    arrow->position = player->position;
    arrow->position.x += direction.x * 8;
    arrow->position.y += direction.y * 8;
    arrow->rotation.z = player->rotation.z + GFC_HALF_PI;
}

void player_bow(Entity* player)
{
    GFC_Vector3D direction;

    direction = cameraPos;
    gfc_vector3d_normalize(&direction);
    direction.z = 0;
    gfc_vector3d_rotate_about_z(&direction, GFC_PI);

    Entity* arrow;
    arrow = entity_new();
    arrow->isProj = 1;
    arrow->isAttack = 1;
    arrow->damage =  + (player->level * 5);
    arrow = gf3d_model_load(arrow, "models/primitives/arrow.model");
    arrow->scale.x = 3;
    arrow->position = player->position;
    arrow->rotation.z = player->rotation.z + GFC_HALF_PI;
    arrow->velocity.x = direction.x * (70 + (player->level * 10));
    arrow->velocity.y = direction.y * (70 + (player->level * 10));
    arrow->velocity.z = 15;
}

void player_flamethrower(Entity* player)
{
    for (int i = 0; i < (4 + player->level); i++)
    {
        GFC_Vector3D direction;

        direction = cameraPos;
        gfc_vector3d_normalize(&direction);
        direction.z = 0;
        gfc_vector3d_rotate_about_z(&direction, GFC_PI);

        direction.x += (gfc_random() - 0.5f) * GFC_HALF_PI * 0.5f;
        direction.y += (gfc_random() - 0.5f) * GFC_HALF_PI * 0.5f;

        Entity* fire;
        fire = entity_new();
        fire->isProj = 1;
        fire->isAttack = 1;
        fire->damage = 5 + player->level;
        fire = gf3d_model_load(fire, "models/primitives/sphere.model");
        fire->position = player->position;
        fire->velocity.x = direction.x * 70;
        fire->velocity.y = direction.y * 70;
        fire->velocity.z = 35 * (gfc_random() - 0.1f);
    }
}

void player_bomb(Entity* player)
{
    slog("player Bomb");
}

void player_rocks(Entity* player)
{
    for (int i = 0; i < (4 + player->level); i++)
    {
        GFC_Vector3D direction;

        direction = cameraPos;
        gfc_vector3d_normalize(&direction);
        direction.z = 0;
        gfc_vector3d_rotate_about_z(&direction, GFC_PI);

        direction.x += (gfc_random() - 0.5f) * GFC_HALF_PI * 0.5f;
        direction.y += (gfc_random() - 0.5f) * GFC_HALF_PI * 0.5f;

        Entity* rock;
        rock = entity_new();
        rock->isProj = 1;
        rock->isAttack = 1;
        rock->isRock = 1;
        rock->damage = 5 + player->level;
        rock = gf3d_model_load(rock, "models/primitives/rock.model");
        rock->position = player->position;
        rock->scale.x = 1 + (player->level * 0.2f);
        rock->scale.y = 1 + (player->level * 0.2f);
        rock->scale.z = 1 + (player->level * 0.2f);
        rock->velocity.x = (gfc_random() - 0.5f) * 10;
        rock->velocity.y = (gfc_random() - 0.5f) * 10;
        rock->velocity.z = 35 * (gfc_random() - 0.1f);
    }
}

GFC_Vector2D borderPos = { 331, 603 };

void player_think(Entity* player)
{
    GFC_Vector3D direction;

    direction = cameraPos;
    gfc_vector3d_normalize(&direction);
    direction.z = 0;
    gfc_vector3d_rotate_about_z(&direction, GFC_PI);

    if (gfc_input_key_down("w"))
    {
        gfc_vector3d_add(player->position, player->position, direction);
    }
    if (gfc_input_key_down("s"))
    {
        gfc_vector3d_rotate_about_z(&direction, GFC_PI);
        gfc_vector3d_add(player->position, player->position, direction);
    }    
    if (gfc_input_key_down("a"))
    {
        gfc_vector3d_rotate_about_z(&direction, GFC_HALF_PI);
        gfc_vector3d_add(player->position, player->position, direction);
    }
    if (gfc_input_key_down("d"))
    {
        gfc_vector3d_rotate_about_z(&direction, -1 * GFC_HALF_PI);
        gfc_vector3d_add(player->position, player->position, direction);
    }
    if (gfc_input_key_pressed("c"))
    {
        player->position.z += 0.01f;
        player->velocity.z = 25;
    }
    if (gf2d_mouse_moved())
    {
        // TODO: Add vertical camera rotation by changing camera to be an entity
        gfc_vector3d_rotate_about_z(&cameraPos, -0.01 * gf2d_mouse_get_movement().x);
        player->rotation.z += -0.01 * gf2d_mouse_get_movement().x;
    }

    if (gfc_input_key_pressed("p"))
    {
        SDL_SetRelativeMouseMode(SDL_TRUE);
    }
    if (gfc_input_key_pressed("o"))
    {
        SDL_SetRelativeMouseMode(SDL_FALSE);
    }   

    if (gfc_input_key_pressed("6"))
    {
        Enemy* enemy;
        enemy = enemy_new();
        enemy_create_walker(enemy);
    }
    
    if (gfc_input_key_pressed("7"))
    {
        Enemy* enemy;
        enemy = enemy_new();
        enemy_create_shooter(enemy);
    }
        
    if (gfc_input_key_pressed("8"))
    {
        Enemy* enemy;
        enemy = enemy_new();
        enemy_create_jumper(enemy);
    }
            
    if (gfc_input_key_pressed("9"))
    {
        Enemy* enemy;
        enemy = enemy_new();
        enemy_create_climber(enemy);
    }
                
    if (gfc_input_key_pressed("0"))
    {
        Enemy* enemy;
        enemy = enemy_new();
        enemy_create_boss(enemy);
    }

    if (player->health <= 0)
    {
        player_die(player);
    }

    if (player->exp >= player->exp_goal)
    {
        player->level += 1;
        player->exp -= player->exp_goal;
        player->exp_goal *= 1.2f;
        slog("Level UP");
    }

    if (gf2d_mouse_button_pressed(0))
    {
        player->attack(player);
    }

    if (gfc_input_key_pressed("1"))
    {
        player->attack = player_sword;
        borderPos = gfc_vector2d(331, 603);
    }
    if (gfc_input_key_pressed("2"))
    {
        player->attack = player_bow;
        borderPos = gfc_vector2d(459, 603);
    }
    if (gfc_input_key_pressed("3"))
    {
        player->attack = player_flamethrower;
        borderPos = gfc_vector2d(587, 603);
    }
    if (gfc_input_key_pressed("4"))
    {
        player->attack = player_bomb;
        borderPos = gfc_vector2d(715, 603);
    }
    if (gfc_input_key_pressed("5"))
    {
        player->attack = player_rocks;
        borderPos = gfc_vector2d(843, 603);
    }

    if (gfc_input_key_pressed("i"))
    {
        player->exp = player->exp_goal;
    }
    
    if (gfc_input_key_pressed("u"))
    {
        player->exp = 0;
        player->exp_goal = 100;
        player->level = 0;
    }


}

int main(int argc,char *argv[])
{
    //local variables
    Sprite *healthBar, *health, *exp, *sword, *bow, *flamethrower, *bomb, *rocks, *border;
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

    entity_init(2000);
    enemy_init(1000);

    SDL_SetRelativeMouseMode(SDL_TRUE);
    
    //game init
    srand(SDL_GetTicks());
    slog_sync();
    healthBar = gf2d_sprite_load_image("images/player_ui/HealthBar.png");
    health = gf2d_sprite_load_image("images/player_ui/Health.png");
    exp = gf2d_sprite_load_image("images/player_ui/exp.png");
    sword = gf2d_sprite_load_image("images/player_ui/Sword.png");
    bow = gf2d_sprite_load_image("images/player_ui/Bow.png");
    flamethrower = gf2d_sprite_load_image("images/player_ui/Flamethrower.png");
    bomb = gf2d_sprite_load_image("images/player_ui/Bomb.png");
    rocks = gf2d_sprite_load_image("images/player_ui/Rocks.png");
    border = gf2d_sprite_load_image("images/player_ui/Border.png");

    map_generate_level();

    Mesh* skyMesh;
    Texture* skyTexture;
    MeshUBO skyUBO;
    GFC_Matrix4 skyMat;

    skyMesh = gf3d_mesh_load_obj("models/sky/sky.obj");
    skyTexture = gf3d_texture_load("models/sky/sky.png");
    gfc_matrix4_identity(skyMat);
    skyUBO = gf3d_mesh_get_ubo(skyMat, GFC_COLOR_WHITE);
    
    //Mesh* alphaMesh;
    //Texture* alphaTexture;
    //MeshUBO alphaUBO;
    //GFC_Matrix4 alphaMat;

    //alphaMesh = gf3d_mesh_load_obj("models/primitives/cube.obj");
    //alphaTexture = gf3d_texture_load("models/primitives/flatred.png");
    //gfc_matrix4_identity(alphaMat);
    //alphaUBO = gf3d_mesh_get_ubo(alphaMat, GFC_COLOR_WHITE);

    Entity* transparentTest;
    transparentTest = entity_new();
    transparentTest = gf3d_model_load(transparentTest, "models/primitives/transparent.model");
    transparentTest->position = gfc_vector3d(10, 10, 0);
    transparentTest->scale = gfc_vector3d(20, 20, 20);
    transparentTest->entity_pipe = gf3d_mesh_get_alpha_pipeline();
    transparentTest->ignoreCollisions = 1;

    Entity* player;
    player = entity_new();
    player_init(player);
    player->think = player_think;
    player->attack = player_sword;


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

        camera = gfc_vector3d_added(gfc_vector3d(player->modelMat[3][0], player->modelMat[3][1], player->modelMat[3][2]), cameraPos);
        gf3d_camera_look_at(gfc_vector3d(player->modelMat[3][0], player->modelMat[3][1], player->modelMat[3][2]), &camera);

        gf3d_camera_update_view();

        gf3d_vgraphics_render_start();
                //2D draws
                gf3d_mesh_queue_render(skyMesh, gf3d_mesh_get_sky_pipeline(), &skyUBO, skyTexture);
                entity_draw_all();

                gf2d_sprite_draw_image(border, borderPos, gfc_vector2d(1, 1));
                gf2d_sprite_draw_image(healthBar, gfc_vector2d(10, 10), gfc_vector2d(1, 1));
                gf2d_sprite_draw_image(health, gfc_vector2d(13, 13), gfc_vector2d(player->health / player->max_health, 1));
                gf2d_sprite_draw_image(healthBar, gfc_vector2d(10, 70), gfc_vector2d(1, 1));
                gf2d_sprite_draw_image(exp, gfc_vector2d(13, 73), gfc_vector2d(player->exp / player->exp_goal, 1));
                gf2d_sprite_draw_image(sword, gfc_vector2d(334, 606), gfc_vector2d(1, 1));
                gf2d_sprite_draw_image(bow, gfc_vector2d(462, 606), gfc_vector2d(1, 1));
                gf2d_sprite_draw_image(flamethrower, gfc_vector2d(590, 606), gfc_vector2d(1, 1));
                gf2d_sprite_draw_image(bomb, gfc_vector2d(718, 606), gfc_vector2d(1, 1));
                gf2d_sprite_draw_image(rocks, gfc_vector2d(846, 606), gfc_vector2d(1, 1));

                gf2d_font_draw_line_tag(gfc_stringf("%.0f/%.0f", player->exp, player->exp_goal)->buffer, FT_H1, GFC_COLOR_BLACK, gfc_vector2d(80, 75));
                gf2d_font_draw_line_tag(gfc_stringf("lv: %d", player->level)->buffer, FT_H1, GFC_COLOR_BLACK, gfc_vector2d(10, 115));
                

                
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
