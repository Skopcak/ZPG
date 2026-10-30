#version 410 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 color;
layout (location = 2) in vec3 normal;

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;
uniform float normalMode = 2.0;

out vec4 worldPosition;
out vec3 worldNormal;
out vec3 vertexColor;

void main()
{
    vertexColor = color;
    worldPosition = modelMatrix * vec4(position, 1.0);

    vec3 transformedNormal;

    if (normalMode < 0.5)
    {
        // Keep the normal in local space.
        transformedNormal = normal;
    }
    else if (normalMode < 1.5)
    {
        // Incorrect for non-uniform scaling.
        transformedNormal = mat3(modelMatrix) * normal;
    }
    else
    {
        // Preserve perpendicularity to the transformed surface.
        mat3 normalMatrix = transpose(inverse(mat3(modelMatrix)));
        transformedNormal = normalMatrix * normal;
    }

    worldNormal = normalize(transformedNormal);
    gl_Position = projectionMatrix * viewMatrix * worldPosition;
}