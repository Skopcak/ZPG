#version 410 core

in vec3 vertexColor;
out vec4 out_Color;

uniform vec3 ambientColor = vec3(0.385, 0.647, 0.812);
uniform float useVertexColor = 0.0;

void main()
{
    vec3 surfaceColor = ambientColor;

    if (useVertexColor > 0.5){
        surfaceColor = vertexColor;
    }

    vec3 ambientLight = vec3(1.0);
    vec3 ambient = ambientLight * surfaceColor;

    out_Color = vec4(ambient, 1.0);
}