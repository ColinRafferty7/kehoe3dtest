#include "simple_logger.h"

#include "entity.h"

typedef struct
{
    Entity* ent_list;
    Uint32 ent_max;
}EntitySystem;

EntitySystem ent_system = { 0 };

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
    if (!ent_system.ent_list) return;

    for (int i = 0; i < ent_system.ent_max; i++)
    {
        entity_think(&ent_system.ent_list[i]);
    }
}

void entity_draw(Entity* ent)
{
    ent->modelUBO = gf3d_mesh_get_ubo(ent->modelMat, GFC_COLOR_WHITE);
    gf3d_mesh_queue_render(ent->modelMesh, gf3d_mesh_get_pipeline(), &ent->modelUBO, ent->modelTexture);
}

void entity_draw_all()
{
    if (!ent_system.ent_list) return;

    for (int i = 0; i < ent_system.ent_max; i++)
    {
        if (ent_system.ent_list[i]._inuse)
        {
            entity_draw(&ent_system.ent_list[i]);
        }
    }
}
