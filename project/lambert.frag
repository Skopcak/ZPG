#version 410 core

in vec4 worldPosition;
in vec3 worldNormal;

out vec4 out_Color;

void main()
{
    vec3 lightPosition = vec3(10.0, 10.0, 10.0);
    vec3 lightDirection = normalize(lightPosition - worldPosition.xyz);
    vec3 normal = normalize(worldNormal);
    float dotProduct = max(dot(normal, lightDirection), 0.0);
    vec3 diffuse = dotProduct * vec3(0.385, 0.647, 0.812);
    out_Color = vec4(diffuse, 1.0);
}
