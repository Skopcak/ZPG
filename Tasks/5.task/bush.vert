#version 410 core



layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;

out vec3 vertexColor;

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;

void main()
{
    float shade = mix(0.3, 1.0, abs(normal.y));

    vertexColor = vec3(0.08, 0.35, 0.10) * shade;

    gl_Position = projectionMatrix * viewMatrix * modelMatrix * vec4(position, 1.0);
}
