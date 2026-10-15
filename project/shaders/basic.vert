#version 410 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 color;

out vec3 vertexColor;

uniform mat4 modelMatrix;
uniform mat4 viewMatrix = mat4(1.0);
uniform mat4 projectionMatrix = mat4(1.0);

void main()
{
    vertexColor = abs(color);

    // Apply the object's transformation to each vertex.
   gl_Position = projectionMatrix * viewMatrix * modelMatrix * vec4(position, 1.0);

}