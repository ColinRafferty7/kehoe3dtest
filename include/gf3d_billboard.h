#ifndef __GF3D_BILLBOARD_H__
#define __GF3D_BILLBOARD_H__

#include "entity.h"

Entity* gf3d_billboard_load(Entity* ent, char* filename);

Mesh* gf3d_billboard_get_mesh();

#endif