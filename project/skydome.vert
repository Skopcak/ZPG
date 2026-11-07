#version 410 core
layout (location = 0) in vec3 position;
layout (location = 1) in vec2 textureCoordinate;
uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;
out vec2 uv;

void main(){
    uv = textureCoordinate;
    vec4 clip = projectionMatrix * mat4(mat3(viewMatrix)) * vec4(position, 1.0);
    gl_Position = clip.xyww;
}
