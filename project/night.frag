#version 410 core

in vec4 worldPosition;
in vec3 worldNormal;
in vec3 vertexColor;

out vec4 out_Color;

uniform vec3 cameraPosition;
uniform vec3 lightPosition;
uniform vec3 lightColor;

uniform vec3 materialColor;
uniform float useVertexColor;
uniform float shininess;
uniform float specularStrength;

uniform vec3 attenuationFactors;

float attenuation(float distanceToLight){
    float denominator =
        attenuationFactors.x + attenuationFactors.y * distanceToLight + attenuationFactors.z * distanceToLight * distanceToLight;
    return clamp(1.0 / denominator, 0.0, 1.0);
}

void main(){
    vec3 surfaceColor = materialColor;

    if (useVertexColor > 0.5){
        surfaceColor = vertexColor;
    }

    vec3 toLight = lightPosition - worldPosition.xyz;
    float distanceToLight = length(toLight);

    vec3 lightDirection =
        toLight / max(distanceToLight, 0.0001);

    vec3 normal = normalize(worldNormal);
    vec3 viewDirection =
        normalize(cameraPosition - worldPosition.xyz);

    float diffuseFactor =
        max(dot(normal, lightDirection), 0.0);

    float specularFactor = 0.0;

    if (diffuseFactor > 0.0)
    {
        vec3 reflectionDirection =
            reflect(-lightDirection, normal);

        specularFactor = pow(
            max(dot(viewDirection, reflectionDirection), 0.0),
            shininess
        );
    }

    vec3 ambient = 0.2 * surfaceColor;
    vec3 diffuse = diffuseFactor * surfaceColor;
    vec3 specular = specularStrength * specularFactor * vec3(1.0);

    vec3 directLight = attenuation(distanceToLight)
        * lightColor
        * (diffuse + specular);

    out_Color = vec4(ambient + directLight, 1.0);
}
