#include <stdalign.h>

#include "simple_logger.h"

#include "gf3d_buffers.h"
#include "gf3d_swapchain.h"
#include "gf3d_vgraphics.h"
#include "gf3d_pipeline.h"
#include "gf3d_commands.h"
#include "gf3d_mesh.h"
#include "gf3d_obj_load.h"
#include "gf2d_camera.h"

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

void gf3d_mesh_free_all()
{
    int i;
    for (i = 0; i < gf3d_mesh.mesh_max; i++)
    {
        gf3d_mesh_delete(&gf3d_mesh.mesh_list[i]);
    }
}

void gf3d_mesh_close()
{
    if (gf3d_mesh.mesh_list)
    {
        gf3d_mesh_free_all();
        // TODO: iterate through mesh data and free all data
        free(gf3d_mesh.mesh_list);
        gf3d_mesh.mesh_list = NULL;
    }
}

void gf3d_mesh_init(Uint32 mesh_max)
{
    Uint32 att_count = 0;

    if (!mesh_max)
    {
        slog("Cannot initialize mesh max of 0");
        return;
    }

    gf3d_mesh.mesh_max = mesh_max;

    gf3d_mesh.bindingDescription.binding = 0;
    gf3d_mesh.bindingDescription.stride = sizeof(Vertex);
    gf3d_mesh.bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

    gf3d_mesh.attributeDescription[0].binding = 0;
    gf3d_mesh.attributeDescription[0].location = 0;
    gf3d_mesh.attributeDescription[0].format = VK_FORMAT_R32G32B32_SFLOAT;
    gf3d_mesh.attributeDescription[0].offset = offsetof(Vertex, vertex);

    gf3d_mesh.attributeDescription[1].binding = 0;
    gf3d_mesh.attributeDescription[1].location = 1;
    gf3d_mesh.attributeDescription[1].format = VK_FORMAT_R32G32B32_SFLOAT;
    gf3d_mesh.attributeDescription[1].offset = offsetof(Vertex, normal);

    gf3d_mesh.attributeDescription[2].binding = 0;
    gf3d_mesh.attributeDescription[2].location = 2;
    gf3d_mesh.attributeDescription[2].format = VK_FORMAT_R32G32_SFLOAT;
    gf3d_mesh.attributeDescription[2].offset = offsetof(Vertex, texel);

    gf3d_mesh.mesh_list = gfc_allocate_array(sizeof(Mesh), mesh_max);

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

    gf3d_mesh.pipe = gf3d_pipeline_create_from_config(
        gf3d_vgraphics_get_default_logical_device(),
        "config/mesh_pipeline.cfg",
        gf3d_vgraphics_get_view_extent(),
        mesh_max,
        gf3d_mesh_get_bind_description(),
        gf3d_mesh_get_attribute_descriptions(NULL),
        att_count,
        sizeof(MeshUBO),
        VK_INDEX_TYPE_UINT16
    );

    atexit(gf3d_mesh_close);
}

Mesh* gf3d_mesh_new()
{
    for (int i = 0; i < gf3d_mesh.mesh_max; i++)
    {
        if (!gf3d_mesh.mesh_list[i]._inuse)
        {
            return gf3d_mesh_list_add(i);
        }
    }
    for (int i = 0; i < gf3d_mesh.mesh_max; i++)
    {
        if (!gf3d_mesh.mesh_list[i]._refCount)
        {
            gf3d_mesh_delete(&gf3d_mesh.mesh_list[i]);
            return gf3d_mesh_list_add(i);
        }
    }
    return NULL;
}

Mesh* gf3d_mesh_load_obj(const char* filename)
{
    Mesh *mesh;
    MeshPrimitive *primitive;
    ObjData *object = NULL;

    mesh = gf3d_mesh_get_by_filename(filename);
    if (mesh) return mesh;

    object = gf3d_obj_load_from_file(filename);
    if (!object) return NULL;
    
    mesh = gf3d_mesh_new();
    if (!mesh) return NULL;

    gfc_line_cpy(mesh->filename, filename);

    primitive = gf3d_mesh_primitive_new();
    primitive->objData = object;
    gf3d_mesh_create_vertex_buffer_from_vertices(primitive);

    gfc_list_append(mesh->primitives, primitive);
    memcpy(&mesh->bounds, &object->bounds, sizeof(GFC_Box));

    return mesh;
}

Mesh* gf3d_mesh_copy(Mesh* in);

void gf3d_mesh_move_vertices(Mesh* in, GFC_Vector3D offset, GFC_Vector3D rotation);

MeshPrimitive* gf3d_mesh_primitive_new()
{
    MeshPrimitive *primitive = NULL;
    primitive = gfc_allocate_array(sizeof(MeshPrimitive), 1);
    return primitive;
}

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

void gf3d_mesh_render(Mesh* mesh, VkCommandBuffer commandBuffer, VkDescriptorSet* descriptorSet);

void gf3d_mesh_render_generic(Mesh* mesh, Pipeline* pipe, VkDescriptorSet* descriptorSet);

void gf3d_mesh_create_vertex_buffer_from_vertices(MeshPrimitive* primitive)
{
    void* data = NULL;
    VkDevice device = gf3d_vgraphics_get_default_logical_device();
    Vertex* vertices;
    Uint32 vcount;
    Face* faces;
    Uint32 fcount;
    size_t bufferSize;
    VkBuffer stagingBuffer;
    VkDeviceMemory stagingBufferMemory;

    if (!primitive)
    {
        slog("no mesh primitive provided");
        return;
    }

    vertices = primitive->objData->faceVertices;
    vcount = primitive->objData->face_vert_count;
    faces = primitive->objData->outFace;
    fcount = primitive->objData->face_count;
    bufferSize = sizeof(Vertex) * vcount;

    gf3d_buffer_create(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, &stagingBuffer, &stagingBufferMemory);

    vkMapMemory(device, stagingBufferMemory, 0, bufferSize, 0, &data);
    memcpy(data, vertices, (size_t)bufferSize);
    vkUnmapMemory(device, stagingBufferMemory);

    gf3d_buffer_create(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, &primitive->vertexBuffer, &primitive->vertexBufferMemory);

    gf3d_buffer_copy(stagingBuffer, primitive->vertexBuffer, bufferSize);

    vkDestroyBuffer(device, stagingBuffer, NULL);
    vkFreeMemory(device, stagingBufferMemory, NULL);

    primitive->vertexCount = vcount;

    gf3d_mesh_setup_face_buffers(primitive, faces, fcount);
}

Pipeline* gf3d_mesh_get_pipeline()
{
    return gf3d_mesh.pipe;
}

Pipeline* gf3d_mesh_get_sky_pipeline()
{
    return gf3d_mesh.sky_pipe;
}

MeshUBO gf3d_mesh_get_ubo(
    GFC_Matrix4 modelMat,
    GFC_Color colorMod)
{
    ModelViewProjection mvp;
    MeshUBO meshUBO = { 0 };
    GFC_Vector4D color = gfc_color_to_vector4f(colorMod);

    mvp = gf3d_vgraphics_get_mvp();
    gfc_matrix4_copy(meshUBO.model, modelMat);
    gfc_matrix4_copy(meshUBO.view, mvp.view);
    gfc_matrix4_copy(meshUBO.proj, mvp.proj);
    gfc_vector4d_copy(meshUBO.color, color);

    meshUBO.camera = gfc_vector2dzw(gf2d_camera_get_position(), 0.0, 1.0);

    return meshUBO;
}

Mesh *gf3d_mesh_get_by_filename(const char *filename)
{
    for (int i = 0; i < gf3d_mesh.mesh_max; i++)
    {
        if (gfc_line_cmp(gf3d_mesh.mesh_list[i].filename, filename) == 0)
        {
            return &gf3d_mesh.mesh_list[i];
        }
    }
    return NULL;
}

Mesh* gf3d_mesh_list_add(int index)
{
    gf3d_mesh.mesh_list[index]._inuse = 1;
    gf3d_mesh.mesh_list[index]._refCount = 1;
    gf3d_mesh.mesh_list[index].primitives = gfc_list_new();
    return &gf3d_mesh.mesh_list[index];
}

void gf3d_mesh_delete(Mesh *mesh)
{
    int count;
    MeshPrimitive* primitive;

    if (!mesh || !mesh->_inuse) return;

    count = gfc_list_get_count(mesh->primitives);

    for (int i = 0; i < count; i++)
    {
        primitive = gfc_list_get_nth(mesh->primitives, i);
        if (!primitive) continue;
        gf3d_mesh_primitive_free(primitive);
    }
    gfc_list_delete(mesh->primitives);
    memset(mesh, 0, sizeof(Mesh));
}

void gf3d_mesh_primitive_free(MeshPrimitive* primitive)
{
    if (!primitive) return;
    gf3d_mesh_primitive_delete_buffers(primitive);
    if (primitive->objData)
    {
        gf3d_obj_free(primitive->objData);
    }
    free(primitive);
}

void gf3d_mesh_primitive_delete_buffers(MeshPrimitive* primitive)
{
    if (!primitive)return;
    if (primitive->faceBuffer != VK_NULL_HANDLE)
    {
        vkDestroyBuffer(gf3d_vgraphics_get_default_logical_device(), primitive->faceBuffer, NULL);
        primitive->faceBuffer = VK_NULL_HANDLE;
    }
    if (primitive->faceBufferMemory != VK_NULL_HANDLE)
    {
        vkFreeMemory(gf3d_vgraphics_get_default_logical_device(), primitive->faceBufferMemory, NULL);
        primitive->faceBufferMemory = VK_NULL_HANDLE;
    }
    if (primitive->vertexBuffer != VK_NULL_HANDLE)
    {
        vkDestroyBuffer(gf3d_vgraphics_get_default_logical_device(), primitive->vertexBuffer, NULL);
        primitive->vertexBuffer = VK_NULL_HANDLE;
    }
    if (primitive->vertexBufferMemory != VK_NULL_HANDLE)
    {
        vkFreeMemory(gf3d_vgraphics_get_default_logical_device(), primitive->vertexBufferMemory, NULL);
        primitive->vertexBufferMemory = VK_NULL_HANDLE;
    }
}

void gf3d_mesh_setup_face_buffers(MeshPrimitive* mesh, Face* faces, Uint32 fcount)
{
    void* data;
    VkDevice device = gf3d_vgraphics_get_default_logical_device();
    VkDeviceSize bufferSize = sizeof(Face) * fcount;

    VkBuffer stagingBuffer;
    VkDeviceMemory stagingBufferMemory;

    gf3d_buffer_create(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, &stagingBuffer, &stagingBufferMemory);

    vkMapMemory(device, stagingBufferMemory, 0, bufferSize, 0, &data);
    memcpy(data, faces, (size_t)bufferSize);
    vkUnmapMemory(device, stagingBufferMemory);

    gf3d_buffer_create(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, &mesh->faceBuffer, &mesh->faceBufferMemory);

    gf3d_buffer_copy(stagingBuffer, mesh->faceBuffer, bufferSize);

    mesh->faceCount = fcount;
    vkDestroyBuffer(device, stagingBuffer, NULL);
    vkFreeMemory(device, stagingBufferMemory, NULL);
}

void gf3d_mesh_queue_render(Mesh* mesh, Pipeline* pipe, void* uboData, Texture* texture)
{
    int count;
    MeshPrimitive* primitive;
    if (!mesh)
    {
        slog("cannot render a NULL mesh");
        return;
    }
    if (!pipe)
    {
        slog("cannot render with NULL pipe");
        return;
    }
    if (!uboData)
    {
        slog("cannot render with NULL descriptor set");
        return;
    }
    count = gfc_list_get_count(mesh->primitives);
    for (int i = 0; i < count; i++)
    {
        primitive = gfc_list_get_nth(mesh->primitives, i);
        if (!primitive)continue;
        gf3d_pipeline_queue_render(
            pipe,
            primitive->vertexBuffer,
            primitive->faceCount * 3,
            primitive->faceBuffer,
            uboData,
            texture);
    }
}

/*eol@eof*/