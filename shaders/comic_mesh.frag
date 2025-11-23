#version 450
#extension GL_ARB_separate_shader_objects : enable

struct MeshUBO
{
    mat4    model;
    mat4    view;
    mat4    proj;
    vec4    color;
    vec4    camera; // xyz = camera position
};

layout(binding = 0) uniform UniformBufferObject
{
    MeshUBO mesh;
} ubo;

layout(binding = 1) uniform sampler2D texSampler;

layout(location = 0) in vec3 fragNormal;
layout(location = 1) in vec2 fragTexCoord;
layout(location = 2) in vec3 fragPos;

layout(location = 0) out vec4 outColor;

void main()
{
    // Shading Variables
    int bandCount = 3;
    float outlineWidth = 0.2;

    // Lighting parameters
    vec3 lightPos = vec3(-100.0, -100.0, 200.0);
    vec3 lightColor = vec3(1.0, 1.0, 1.0);

    vec3 norm = normalize(fragNormal);
    vec3 lightDir = normalize(lightPos - fragPos);
    vec3 viewDir = normalize(ubo.mesh.camera.xyz - fragPos);

    // Ambient light (soft baseline)
    vec3 ambient = 0.15 * lightColor;

    // Diffuse light (Lambert)
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * lightColor;

    // Combine all
    vec3 lighting = (ambient + diffuse);

    // Apply texture and tint
    vec4 surfaceColor = texture(texSampler, fragTexCoord) * ubo.mesh.color;
    // vec3 finalColor = surfaceColor.rgb * lighting;
    vec3 finalColor = vec3(surfaceColor.r * (round(lighting.r * bandCount) / bandCount + 2 / bandCount), 
    surfaceColor.g * (round(lighting.g * bandCount) / bandCount + 2 / bandCount), 
    surfaceColor.b * (round(lighting.b * bandCount) / bandCount + 2 / bandCount));

    vec3 camDist = normalize(ubo.mesh.camera.xyz - fragPos);
    float dotProd = dot(fragNormal, camDist);

    if (dotProd < 0.2)
    {
        outColor = vec4(0.0, 0.0, 0.0, 1.0);
    }
    else
    {
        outColor = vec4(finalColor, surfaceColor.a);
    }
}
