#version 410 core

out vec4 out_Color;

uniform vec3 ambientColor = vec3(0.385, 0.647, 0.812);

void main()
{
    out_Color = vec4(ambientColor, 1.0);
}
