#version 410 core
in vec2 uv;
uniform sampler2D skyTexture;
out vec4 FragColor;

void main()
{
    FragColor = texture(skyTexture, uv);
}
