#include "simple_logger.h"
#include "gf3d_billboard.h"

Mesh* billboardMesh;

Entity* gf3d_billboard_load(Entity* ent, char* filename)
{
	ent->modelMesh = gf3d_billboard_get_mesh();

	if (!ent->modelMesh)
	{
		slog("No mesh loaded");
		return NULL;
	}

	ent->modelTexture = gf3d_texture_load(filename);
	if (!ent->modelTexture)
	{
		slog("No Texture");
		return NULL;
	}

	gfc_matrix4_identity(ent->modelMat);
	gfc_matrix4_scale(ent->modelMat, ent->modelMat, gfc_vector3d(1, 1, 1));

	ent->ignoreCollisions = 1;

	return ent;
}

Mesh* gf3d_billboard_get_mesh()
{
	if (billboardMesh) return billboardMesh;
	
	billboardMesh = gf3d_mesh_new();

	billboardMesh = gf3d_mesh_load_obj("models/primitives/billboard.obj");

	return billboardMesh;
}