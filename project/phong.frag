#version 410 core

in vec4 worldPosition;
in vec3 worldNormal;

out vec4 out_Color;
uniform vec3 cameraPosition;

void main()
{
    vec3 lightPosition = vec3(10.0, 10.0, 10.0);

    // Direction from the surface toward the light.
    vec3 lightToVector = lightPosition - worldPosition.xyz;

    float dotProduct = max(
        dot(normalize(lightToVector), normalize(worldNormal)),
        0.0
    );

    vec4 diffuse = dotProduct * vec4(0.385, 0.647, 0.812, 1.0);
    vec4 ambient = vec4(0.1, 0.1, 0.1, 1.0);

    vec3 normal = normalize(worldNormal);
    vec3 lightDirection = normalize(lightToVector);
    vec3 viewDirection = normalize(cameraPosition - worldPosition.xyz);

    // Reflect the incoming light direction around the normal.
    vec3 reflectionDirection = reflect(-lightDirection, normal);

    float specularFactor = 0.0;

    if (dotProduct > 0.0)
    {
        specularFactor = pow(
            max(dot(viewDirection, reflectionDirection), 0.0),
            32.0
        );
    }

    vec3 specular = 0.5 * specularFactor * vec3(1.0);

    out_Color = vec4(ambient.rgb + diffuse.rgb + specular, 1.0);
}
