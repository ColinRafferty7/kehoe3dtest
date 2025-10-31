#include "simple_logger.h"
#include "gf3d_model.h"


Entity* gf3d_model_load(Entity* ent, char* filename)
{
	SJson* json;
	SJson* config;
	const char* meshFile;
	const char* textureFile;

	json = sj_load(filename);
	if (!json)
	{
		slog("No JSON");
		return NULL;
	}

	config = sj_object_get_value(json, "model");

	meshFile = sj_get_string_value(sj_object_get_value(config, "obj"));
	if (!meshFile)
	{
		slog("No MeshFile");
		return NULL;
	}

	ent->modelMesh = gf3d_mesh_load_obj(meshFile);
	if (!ent->modelMesh)
	{
		slog("No Mesh");
		return NULL;
	}

	textureFile = sj_get_string_value(sj_object_get_value(config, "texture"));
	if (!textureFile)
	{
		slog("No TextureFile");
		return NULL;
	}

	ent->modelTexture = gf3d_texture_load(textureFile);
	if (!ent->modelTexture)
	{
		slog("No Texture");
		return NULL;
	}

	gfc_matrix4_identity(ent->modelMat);
	gfc_matrix4_scale(ent->modelMat, ent->modelMat, gfc_vector3d(1, 1, 1));

	return ent;
}