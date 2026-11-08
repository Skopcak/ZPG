#version 410 core

layout (location = 0) in vec3 position;
layout (location = 2) in vec2 textureCoordinate;

uniform vec3 viewportSize;
uniform mat4 modelMatrix;
out vec2 uv;

void main()
{
    // Fixed pixel size and margin keep the login readable after resizing.
    vec2 pixel = vec2(viewportSize.x - 208.0, viewportSize.y - 64.0)
               + (modelMatrix * vec4(position, 1.0)).xy * vec2(192.0, 48.0);
    gl_Position = vec4(2.0 * pixel / viewportSize.xy - 1.0, 0.0, 1.0);
    uv = textureCoordinate;
}
