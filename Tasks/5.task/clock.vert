#version 410 core


layout (location = 0) in vec3 position;
layout (location = 2) in vec3 color;

out vec3 vertexColor;

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;

uniform mat4 hourMatrix = mat4(1.0);
uniform mat4 minuteMatrix = mat4(1.0);
uniform mat4 secondMatrix = mat4(1.0);

void main()
{
    vec4 transformedPosition = vec4(position, 1.0);
    vertexColor = color;

    // Identify each hand using its original vertex color.
    if (color == vec3(0.0, 0.0, 1.0)){
        transformedPosition = hourMatrix * transformedPosition;
        vertexColor = vec3(0.0);
    }
    else if (color == vec3(0.0, 1.0, 0.0)){
        transformedPosition = minuteMatrix * transformedPosition;
        vertexColor = vec3(0.0);
    }
    else if (color == vec3(1.0, 0.0, 0.0)){
        transformedPosition = secondMatrix * transformedPosition;
        vertexColor = vec3(1.0, 0.0, 0.0);
    }
    else if (color == vec3(1.0, 1.0, 0.0)){
        vertexColor = vec3(5.0, 195.0, 222.0) / 255.0;
    }

    gl_Position = projectionMatrix * viewMatrix * modelMatrix * transformedPosition;
}
