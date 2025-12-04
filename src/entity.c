#include "simple_logger.h"

#include "entity.h"
#include "collision.h"
#include <gf3d_obj_load.h>
#include "scene.h"

typedef struct
{
    Entity* ent_list;
    Uint32 ent_max;
}EntitySystem;

static EntitySystem ent_system = { 0 };

void entity_close()
{
    if (!ent_system.ent_list) return;

    for (int i = 0; i < ent_system.ent_max; i++)
    {
        free(&ent_system.ent_list[i]);
    }
    free(ent_system.ent_list);
}

void entity_init(Uint32 entity_max)
{
    if (!entity_max)
    {
        slog("Cannot initialize entity max of 0");
        return;
    }

    ent_system.ent_list = gfc_allocate_array(sizeof(Entity), entity_max);
    ent_system.ent_max = entity_max;

    atexit(entity_close);
}

Entity* entity_new()
{
    for (int i = 0; i < ent_system.ent_max; i++)
    {
        if (ent_system.ent_list[i]._inuse) continue;

        memset(&ent_system.ent_list[i], 0, sizeof(Entity));
        ent_system.ent_list[i]._inuse = 1;
        ent_system.ent_list[i].position = gfc_vector3d(0, 0, 0);
        ent_system.ent_list[i].scale = gfc_vector3d(1, 1, 1);
        ent_system.ent_list[i].rotation = gfc_vector3d(0, 0, 0);
        ent_system.ent_list[i].boundingBox = gfc_box(0, 0, 0, 0, 0, 0);

        scene_add_entity(scene_get_active(), &ent_system.ent_list[i]);
     
        return &ent_system.ent_list[i];
    }

    return NULL;
}

void entity_think(Entity* ent)
{
    if (!ent) return;

    if (ent->think)
    {
        ent->think(ent);
    }
}

void entity_think_all()
{
    Scene* active_scene = scene_get_active();
    if (!active_scene) return;

    for (int i = 0; i < active_scene->ent_max; i++)
    {
        if (!active_scene->entities[i]) continue;

        if (active_scene->entities[i]->_inuse)
        {
            entity_think(active_scene->entities[i]);
        }
    }
}

void entity_draw(Entity* ent)
{
    ent->modelUBO = gf3d_mesh_get_ubo(ent->modelMat, GFC_COLOR_WHITE);
    if (ent->entity_pipe)
    {
        gf3d_mesh_queue_render(ent->modelMesh, gf3d_mesh_get_outline_pipeline(), &ent->modelUBO, ent->modelTexture);
        gf3d_mesh_queue_render(ent->modelMesh, ent->entity_pipe, &ent->modelUBO, ent->modelTexture);
    }
    else
    {
        gf3d_mesh_queue_render(ent->modelMesh, gf3d_mesh_get_pipeline(), &ent->modelUBO, ent->modelTexture);
    }
}

void entity_draw_all()
{
    Scene* active_scene = scene_get_active();
    if (!active_scene) return;

    for (int i = 0; i < active_scene->ent_max; i++)
    {
        if (!active_scene->entities[i]) continue;

        if (active_scene->entities[i]->_inuse)
        {
            entity_draw(active_scene->entities[i]);
        }
    }
}

void entity_update(Entity* ent)
{
    if (!ent->isStatic)
    {
        physics_update(ent);
    }

    gfc_matrix4_from_vectors(ent->modelMat, ent->position, ent->rotation, ent->scale);

    ent->boundingBox = gfc_box(
        ent->position.x + (ent->modelMesh->bounds.x * ent->scale.x),
        ent->position.y + (ent->modelMesh->bounds.y * ent->scale.y),
        ent->position.z + (ent->modelMesh->bounds.z * ent->scale.z),
        ent->modelMesh->bounds.w * ent->scale.x,
        ent->modelMesh->bounds.h * ent->scale.y,
        ent->modelMesh->bounds.d * ent->scale.z
    );
}

void entity_update_all()
{
    Scene* active_scene = scene_get_active();
    if (!active_scene) return;

    for (int i = 0; i < active_scene->ent_max; i++)
    {
        if (!active_scene->entities[i]) continue;

        if (active_scene->entities[i]->_inuse)
        {
            entity_update(active_scene->entities[i]);
        }
    }
}

void entity_collision_check_all()
{
    // TODO: Make sublists that only contain entities that should be iterated through

    Scene* active_scene = scene_get_active();
    if (!ent_system.ent_list) return;

    for (int i = 0; i < ent_system.ent_max; i++)
    {
        if (!ent_system.ent_list[i]._inuse || ent_system.ent_list[i].isStatic) continue;
        for (int j = 0; j < ent_system.ent_max; j++)
        {
            if (!ent_system.ent_list[j]._inuse || i == j) continue;
            collision_check(&ent_system.ent_list[i], &ent_system.ent_list[j]);
        }
    }
}

Entity* entity_get_player()
{
    if (!ent_system.ent_list) return;

    for (int i = 0; i < ent_system.ent_max; i++)
    {
        if (ent_system.ent_list[i]._inuse && ent_system.ent_list[i].isPlayer)
        {
            return &ent_system.ent_list[i];
        }
    }
}

void entity_free(Entity* ent)
{
    if (!ent)return;
    ent->_inuse = 0;
    memset(ent, 0, sizeof(Entity));
}