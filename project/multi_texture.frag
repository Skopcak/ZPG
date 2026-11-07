#version 410 core

in vec2 uv;
uniform sampler2D baseTexture;
uniform sampler2D detailTexture;
uniform float detailStrength;
out vec4 FragColor;

void main()
{
    vec4 base = texture(baseTexture, uv);
    vec4 detail = texture(detailTexture, uv * 8.0);
    FragColor = vec4(mix(base.rgb, detail.rgb, detailStrength), base.a);
}
