#ifndef __ENTITY_H__
#define __ENTITY_H__

#include "gf3d_mesh.h"

#define JUMP_FORCE 12.0f

typedef struct Entity_S
{
    Uint8 _inuse;

    const char* name;

    GFC_Vector3D position;
    GFC_Vector3D rotation;
    GFC_Vector3D scale;

    GFC_Vector3D velocity;

    GFC_Box boundingBox;

    Uint8 isStatic;

    void (*think)   (struct Entity_S* ent);

    Mesh* modelMesh;
    Texture* modelTexture;
    MeshUBO modelUBO;
    GFC_Matrix4 modelMat;
}Entity;

void entity_close();

void entity_init(Uint32 entity_max);

Entity* entity_new();

void entity_think_all();

void entity_draw_all();

void entity_update_all();

void entity_collision_check_all();

#endif