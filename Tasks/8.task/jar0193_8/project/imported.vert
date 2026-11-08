#version 410 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec4 color;
layout(location = 2) in vec3 normal;
layout(location = 3) in vec2 textureCoordinate;

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;

out vec3 worldPosition;
out vec3 worldNormal;
out vec4 vertexColor;
out vec2 uv;

void main()
{
    vec4 world = modelMatrix * vec4(position, 1.0);
    worldPosition = world.xyz;
    worldNormal = transpose(inverse(mat3(modelMatrix))) * normal;
    vertexColor = color;
    uv = textureCoordinate;
    gl_Position = projectionMatrix * viewMatrix * world;
}
