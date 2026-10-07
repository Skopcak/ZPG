#version 410 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 color;

out vec3 vertexColor;
uniform vec3 offset = vec3(-0.5, 0.0, 0.0);
uniform float angle;
uniform float scaleFactor;

void main()
{
    // Use absolute values to display normals as colors.
    vertexColor = abs(color);
    // Rotate around the Y axis, then scale and translate.
    vec3 rotatedPosition;

    rotatedPosition.x = cos(angle) * position.x
                       +sin(angle) * position.z;

    rotatedPosition.y = position.y;
    
    rotatedPosition.z = -sin(angle) * position.x
                        + cos(angle) * position.z;

    gl_Position = vec4(rotatedPosition * scaleFactor + offset, 1.0);
}