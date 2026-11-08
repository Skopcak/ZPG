#version 410 core

in vec3 worldPosition;
in vec3 worldNormal;
in vec4 vertexColor;
in vec2 uv;

uniform vec3 cameraPosition;
uniform vec3 lightPosition;
uniform vec3 lightColor;
uniform vec3 attenuationFactors;
uniform vec4 baseColorFactor;
uniform vec3 specularColor;
uniform vec3 emissiveColor;
uniform float shininess;
uniform int alphaMode;
uniform sampler2D colorTexture;
uniform sampler2D emissiveTexture;

out vec4 FragColor;

void main()
{
    vec4 surface = baseColorFactor * vertexColor * texture(colorTexture, uv);
    vec3 normal = normalize(worldNormal);
    if (!gl_FrontFacing) normal = -normal;
    vec3 difference = lightPosition - worldPosition;
    float distanceToLight = length(difference);
    vec3 light = difference / max(distanceToLight, 0.00001);
    vec3 toCamera = cameraPosition - worldPosition;
    vec3 view = toCamera / max(length(toCamera), 0.00001);
    float diffuse = max(dot(normal, light), 0.0);
    float highlight = 0.0;
    if (diffuse > 0.0) {
        float alignment = max(dot(view, reflect(-light, normal)), 0.0);
        if (alignment > 0.0) highlight = pow(alignment, shininess);
    }
    float attenuation = 1.0 / max(dot(attenuationFactors,
        vec3(1.0, distanceToLight, distanceToLight * distanceToLight)), 0.00001);
    vec3 emission = emissiveColor * texture(emissiveTexture, uv).rgb;
    vec3 lit = (surface.rgb * diffuse + specularColor * highlight) * lightColor * attenuation;
    FragColor = vec4(surface.rgb * 0.1 + lit + emission, alphaMode == 2 ? surface.a : 1.0);
}
