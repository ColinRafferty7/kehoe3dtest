#include "scene.h"

#include "simple_logger.h"
#include "gf2d_ui.h"

typedef struct
{
	Scene* scene_list;
	Uint32 scene_max;
}SceneManager;

SceneManager scene_manager = { 0 };

Scene* g_scene_active;

void scene_init(Uint32 scene_max)
{
    if (!scene_max)
    {
        slog("Cannot initialize scene max of 0");
        return;
    }

    scene_manager.scene_list = gfc_allocate_array(sizeof(Scene), scene_max);
    scene_manager.scene_max = scene_max;

    atexit(scene_close);
}

Scene* scene_new()
{
    for (int i = 0; i < scene_manager.scene_max; i++)
    {
        if (scene_manager.scene_list[i]._inuse) continue;

        memset(&scene_manager.scene_list[i], 0, sizeof(Scene));

        scene_manager.scene_list[i]._inuse = 1;
        scene_manager.scene_list[i].ent_max = 1000;
        scene_manager.scene_list[i].entities = gfc_allocate_array(sizeof(Entity*), scene_manager.scene_list[i].ent_max);
        
        return &scene_manager.scene_list[i];
    }

    return NULL;
}

void scene_free(Scene* scene)
{
	free(scene->entities);
	free(scene);
}

void scene_close()
{
    if (!scene_manager.scene_list) return;

    for (int i = 0; i < scene_manager.scene_max; i++)
    {
        scene_free(&scene_manager.scene_list[i]);
    }
    free(scene_manager.scene_list);
}

void scene_add_entity(Scene* scene, Entity* ent)
{
    scene->entities[scene->ent_count++] = ent;
}

void scene_set_active(Scene* scene)
{
    if (g_scene_active)
    {
        for (int i = 0; i < g_scene_active->ent_count; i++)
        {
            if (!g_scene_active->entities[i]->persist)
            {
                entity_free(g_scene_active->entities[i]);
            }
        }
    }
    g_scene_active = scene;
}

Scene* scene_get_active()
{
    return g_scene_active;
}

Scene* scene_main_menu()
{
    Scene *scene;
    scene = scene_new();

}