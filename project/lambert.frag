#version 410 core

in vec4 worldPosition;
in vec3 worldNormal;

out vec4 out_Color;
uniform vec3 lightPosition;

void main()
{
    vec3 lightDirection = normalize(lightPosition - worldPosition.xyz);
    vec3 normal = normalize(worldNormal);
    float dotProduct = max(dot(normal, lightDirection), 0.0);

    vec3 ambient = vec3(0.1);
    vec3 diffuse = dotProduct * vec3(0.385, 0.647, 0.812);

    out_Color = vec4(ambient + diffuse, 1.0);
}
