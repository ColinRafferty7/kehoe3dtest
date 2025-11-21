#version 450
#extension GL_ARB_separate_shader_objects : enable

struct MeshUBO
{
    mat4 model;
    mat4 view;
    mat4 proj;
    vec4 color;
    vec4 camera; // xyz = camera position
};

layout(binding = 0) uniform UniformBufferObject
{
    MeshUBO mesh;
} ubo;

out gl_PerVertex
{
    vec4 gl_Position;
};

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec2 inTexCoord;

layout(location = 0) out vec3 fragNormal;
layout(location = 1) out vec2 fragTexCoord;
layout(location = 2) out vec3 fragPos;

void main()
{
    mat4 mvp = ubo.mesh.proj * ubo.mesh.view * ubo.mesh.model;
    fragTexCoord = inTexCoord;
    gl_Position = mvp * vec4(inPosition, 1.0);
    fragPos = inPosition;
    fragNormal = inNormal;
}
