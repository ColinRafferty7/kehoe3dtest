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

Entity entity_spawn()
{
    Entity ent;

    ent.modelMesh = gf3d_mesh_load_obj("models/dino/dino.obj");
    ent.modelTexture = gf3d_texture_load("models/dino/dino.png");
    gfc_matrix4_identity(ent.modelMat);
    gfc_matrix4_scale(ent.modelMat, ent.modelMat, gfc_vector3d(1, 1, 1));
    gfc_matrix4_rotate_y(ent.modelMat, ent.modelMat, GFC_HALF_PI * -1);
    gfc_matrix4_translate(ent.modelMat, ent.modelMat, gfc_vector3d(0, 0, -50));

    return ent;
}