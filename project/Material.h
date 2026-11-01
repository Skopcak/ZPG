#pragma once

#include <glm/vec3.hpp>

class ShaderProgram;

// Surface parameters, independent of the light's position and intensity.
struct Material
{
    glm::vec3 color{0.1f, 0.35f, 0.1f};
    bool useVertexColor = false;
    float shininess = 16.0f;
    float specularStrength = 0.15f;

    // Requires materialColor, useVertexColor, shininess and specularStrength.
    void apply(const ShaderProgram& program) const;
};
