#ifndef __ENTITY_H__
#define __ENTITY_H__

#include "gf3d_mesh.h"

typedef struct Entity_S
{
    Uint8 _inuse;

    GFC_Vector3D position;
    GFC_Vector3D rotation;
    GFC_Vector3D scale;

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

#endif