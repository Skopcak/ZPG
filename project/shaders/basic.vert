#version 330 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 color;

out vec3 vertexColor;
uniform vec3 offset;


void main()
{
    vertexColor = color ;
    gl_Position = vec4(position * 0.35 + offset, 1.0);
}