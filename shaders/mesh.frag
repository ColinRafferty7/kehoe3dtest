#version 450
#extension GL_ARB_separate_shader_objects : enable

struct MeshUBO
{
    mat4    model;
    mat4    view;
    mat4    proj;
    vec4    color;      // can use for tint
    vec4    camera;     // optional, can be unused for now
};

layout(binding = 0) uniform UniformBufferObject
{
    MeshUBO mesh;
} ubo;

layout(binding = 1) uniform sampler2D texSampler;

layout(location = 0) in vec3 fragNormal;
layout(location = 1) in vec2 fragTexCoord;
layout(location = 2) in vec3 position;

layout(location = 0) out vec4 outColor;

void main()
{
    // Simple texture sampling + optional color tint
    vec4 surfaceColor = texture(texSampler, fragTexCoord);
    outColor = surfaceColor * ubo.mesh.color;
}
