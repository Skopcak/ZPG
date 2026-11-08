#version 410 core
in vec3 direction;
uniform samplerCube skyTexture;
out vec4 FragColor;

void main()
{
    FragColor = texture(skyTexture, direction);
}
