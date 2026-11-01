#pragma once

#include <glm/vec3.hpp>

class ShaderProgram;

// Point light data; attenuation factors are constant, linear and quadratic.
struct Light
{
    glm::vec3 position{0.0f};
    glm::vec3 color{1.0f};
    glm::vec3 attenuationFactors{1.0f, 0.0f, 0.0f};

    // Legacy lighting shaders only expose the light position.
    void applyPosition(const ShaderProgram& program) const;

    // Requires lightPosition, lightColor and attenuationFactors uniforms.
    void apply(const ShaderProgram& program) const;
};
