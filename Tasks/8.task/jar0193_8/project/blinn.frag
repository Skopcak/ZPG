#version 410 core

in vec4 worldPosition;
in vec3 worldNormal;
in vec3 vertexColor;

out vec4 out_Color;
uniform vec3 cameraPosition;
uniform vec3 lightPosition;
uniform float useVertexColor = 0.0;

void main()
{
    // Direction from the surface toward the light.
    vec3 lightToVector = lightPosition - worldPosition.xyz;

    float dotProduct = max(
        dot(normalize(lightToVector), normalize(worldNormal)),
        0.0
    );

    vec3 surfaceColor = vec3(0.385, 0.647, 0.812);
    vec3 ambientColor = vec3(0.1);

    if (useVertexColor > 0.5)
    {
        surfaceColor = vertexColor;
        ambientColor *= surfaceColor;
    }

    vec4 diffuse = dotProduct * vec4(surfaceColor, 1.0);
    vec4 ambient = vec4(ambientColor, 1.0);

    vec3 normal = normalize(worldNormal);
    vec3 lightDirection = normalize(lightToVector);
    vec3 viewDirection = normalize(cameraPosition - worldPosition.xyz);

    // Calculate the halfway direction between light and view.
    vec3 halfVector = normalize(lightDirection + viewDirection);

    float specularFactor = 0.0;

    if (dotProduct > 0.0)
    {
        specularFactor = pow(
            max(dot(normal, halfVector), 0.0),
            32.0
        );
    }

    vec3 specular = 0.5 * specularFactor * vec3(1.0);

    out_Color = vec4(ambient.rgb + diffuse.rgb + specular, 1.0);
}
