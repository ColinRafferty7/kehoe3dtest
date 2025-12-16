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
layout(location = 3) in vec2 screenPos;

layout(location = 0) out vec4 outColor;

void main()
{
    // Shading Variables
    int bandCount = 2;
    float outlineWidth = 0.2;

    vec3 lightPos = vec3(-100.0, -100.0, 200.0);
    vec3 lightColor = vec3(1.0, 1.0, 1.0);

    vec3 norm = normalize(fragNormal);
    vec3 lightDir = normalize(lightPos - fragPos);
    vec3 viewDir = normalize(ubo.mesh.camera.xyz - fragPos);

    vec3 ambient = 0.05 * lightColor;

    float diff = max(dot(fragNormal, lightDir) + 0.3, 0.0);
    diff = min(diff, 1.0);
    vec3 diffuse = diff * lightColor;

    vec3 lighting = (ambient + diffuse);

    vec4 surfaceColor = texture(texSampler, fragTexCoord) * ubo.mesh.color;

    vec3 celLighting = vec3(round(lighting.r * bandCount) / bandCount + 1 / bandCount, 
    round(lighting.g * bandCount) / bandCount + 1 / bandCount, 
    round(lighting.b * bandCount) / bandCount + 1 / bandCount);

    vec3 finalColor;

    float aspectRatio = abs(ubo.mesh.proj[1][1] / ubo.mesh.proj[0][0]);

    float pixelX = screenPos.x * 100 * aspectRatio;
    float pixelY = screenPos.y * 100;

    float halftoneDistance = pow(mod(pixelX + 1, 2) - 1, 2) + pow(mod(pixelY + 1, 2) - 1, 2);

    float crosshatchDistance_up = abs(mod(pixelX, 2) - mod(pixelY, 2));
    float crosshatchDistance_down = abs(mod(pixelX, 2) + mod(pixelY, 2) - 2);

    int test = 0;

    if (test != 1)
    {
        if (celLighting.x > 0.5)
        {
            // Halftone Highlights
            if (halftoneDistance > (lighting.x - 0.75) * 2)
            {
                finalColor = surfaceColor.rgb * (celLighting - 0.5);
            }
            else
            {
                finalColor = surfaceColor.rgb * celLighting;
            }
        }
        else if (lighting.x < 0.25)
        {
            // Cross Hatching Shadows
            if (crosshatchDistance_up > (0.25 - lighting.x) * 3 && crosshatchDistance_down > (0.25- lighting.x) * 3)
            {
                finalColor = surfaceColor.rgb * (celLighting + 0.5);
            }
            else 
            {
                finalColor = surfaceColor.rgb * celLighting;
            }
        }
        else
        {
            finalColor = surfaceColor.rgb * celLighting;
        }
    }
    else 
    {
        finalColor = lighting;
    }

    outColor = vec4(finalColor.rgb, surfaceColor.a);
}
