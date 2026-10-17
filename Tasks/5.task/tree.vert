#version 410 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;

out vec3 vertexColor;

uniform mat4 modelMatrix;
uniform mat4 viewMatrix = mat4(1.0);
uniform mat4 projectionMatrix = mat4(1.0);

void main()
{
    // Separate leaves from the trunk and branches.
    if (gl_VertexID > 36443){
        vertexColor = vec3(0.1, 0.6, 0.1);
    }else{
        vertexColor = vec3(0.4, 0.2, 0.05);
    }
    float shade = mix(0.3, 1.0, abs(normal.y));
    vertexColor = shade * vertexColor;
    gl_Position = projectionMatrix * viewMatrix * modelMatrix * vec4(position, 1.0);
}
