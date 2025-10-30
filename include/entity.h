#ifndef __ENTITY_H__
#define __ENTITY_H__

#include "gf3d_mesh.h"

typedef struct
{
    Mesh* modelMesh;
    Texture* modelTexture;
    MeshUBO modelUBO;
    GFC_Matrix4 modelMat;
}Entity;

void entity_close();

void entity_init(Uint32 entity_max);

Entity entity_spawn();

#endif