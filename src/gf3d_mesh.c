#include <stdalign.h>

#include "simple_logger.h"

#include "gf3d_buffers.h"
#include "gf3d_swapchain.h"
#include "gf3d_vgraphics.h"
#include "gf3d_pipeline.h"
#include "gf3d_commands.h"
#include "gf3d_mesh.h"
#include "gf3d_obj_load.h"

#define ATTRIBUTE_COUNT 3

typedef struct
{
    Mesh *mesh_list;
    Pipeline *pipe;
    Pipeline *sky_pipe;
    Uint32 mesh_max;
    VkVertexInputAttributeDescription attributeDescription[ATTRIBUTE_COUNT];
    VkVertexInputBindingDescription bindingDescription;
    Command* stagingCommandBuffer;
}MeshSystem;

MeshSystem gf3d_mesh = { 0 };

void gf3d_mesh_close()
{
    return;
}

void gf3d_mesh_init(Uint32 mesh_max)
{
    Uint32 att_count = 0;

    if (mesh_max == 0)
    {
        slog("Cannot initialize mesh max of 0");
        return;
    }

    gf3d_mesh.mesh_max = mesh_max;

    gf3d_mesh.bindingDescription.binding = 0;
    gf3d_mesh.bindingDescription.stride = sizeof(Vertex);
    gf3d_mesh.bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
    slog("Binded");

    gf3d_mesh.attributeDescription[0].binding = 0;
    gf3d_mesh.attributeDescription[0].location = 0;
    gf3d_mesh.attributeDescription[0].format = VK_FORMAT_R32G32B32_SFLOAT;
    gf3d_mesh.attributeDescription[0].offset = offsetof(Vertex, vertex);
    slog("Vertex");

    gf3d_mesh.attributeDescription[1].binding = 0;
    gf3d_mesh.attributeDescription[1].location = 1;
    gf3d_mesh.attributeDescription[1].format = VK_FORMAT_R32G32B32_SFLOAT;
    gf3d_mesh.attributeDescription[1].offset = offsetof(Vertex, normal);
    slog("Normal");

    gf3d_mesh.attributeDescription[2].binding = 0;
    gf3d_mesh.attributeDescription[2].location = 2;
    gf3d_mesh.attributeDescription[2].format = VK_FORMAT_R32G32_SFLOAT;
    gf3d_mesh.attributeDescription[2].offset = offsetof(Vertex, texel);
    slog("Normal");

    gf3d_mesh.mesh_list = gfc_allocate_array(sizeof(Mesh), mesh_max);
    slog("Mesh List");

    gf3d_mesh_get_attribute_descriptions(&att_count);

    gf3d_mesh.sky_pipe = gf3d_pipeline_create_from_config(
        gf3d_vgraphics_get_default_logical_device(),
        "config/sky_pipeline.cfg",
        gf3d_vgraphics_get_view_extent(),
        mesh_max,
        gf3d_mesh_get_bind_description(),
        gf3d_mesh_get_attribute_descriptions(NULL),
        att_count,
        sizeof(MeshUBO),
        VK_INDEX_TYPE_UINT16
        );
    slog("Sky Pipe");

    gf3d_mesh.pipe = gf3d_pipeline_create_from_config(
        gf3d_vgraphics_get_default_logical_device(),
        "config/overlay_pipeline.cfg",
        gf3d_vgraphics_get_view_extent(),
        mesh_max,
        gf3d_mesh_get_bind_description(),
        gf3d_mesh_get_attribute_descriptions(NULL),
        att_count,
        sizeof(MeshUBO),
        VK_INDEX_TYPE_UINT16
    );
    slog("Overlay Pipe");

    atexit(gf3d_mesh_close);
}

Mesh* gf3d_mesh_new();

Mesh* gf3d_mesh_load_obj(const char* filename);

Mesh* gf3d_mesh_copy(Mesh* in);

void gf3d_mesh_move_vertices(Mesh* in, GFC_Vector3D offset, GFC_Vector3D rotation);

MeshPrimitive* gf3d_mesh_primitive_new();

VkVertexInputAttributeDescription* gf3d_mesh_get_attribute_descriptions(Uint32* count)
{
    if (count)
    {
        *count = ATTRIBUTE_COUNT;
    }
    return gf3d_mesh.attributeDescription;
}

VkVertexInputBindingDescription* gf3d_mesh_get_bind_description()
{
    return &gf3d_mesh.bindingDescription;
}

void gf3d_mesh_free(Mesh* mesh);

void gf3d_mesh_reset_pipes();

void gf3d_mesh_submit_pipe_commands();

VkCommandBuffer gf3d_mesh_get_model_command_buffer();

void gf3d_mesh_queue_render(Mesh* mesh, Pipeline* pipe, void* uboData, Texture* texture);

void gf3d_mesh_render(Mesh* mesh, VkCommandBuffer commandBuffer, VkDescriptorSet* descriptorSet);

void gf3d_mesh_render_generic(Mesh* mesh, Pipeline* pipe, VkDescriptorSet* descriptorSet);

void gf3d_mesh_create_vertex_buffer_from_vertices(MeshPrimitive* primitive);

Pipeline* gf3d_mesh_get_pipeline()
{
    return NULL;
}

MeshUBO gf3d_mesh_get_ubo(
    GFC_Matrix4 modelMat,
    GFC_Color colorMod);

/*eol@eof*/