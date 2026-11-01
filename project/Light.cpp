#include "Light.h"
#include "ShaderProgram.h"

void Light::applyPosition(const ShaderProgram& program) const
{
    program.setUniform("lightPosition", position.x, position.y, position.z);
}

void Light::apply(const ShaderProgram& program) const
{
    applyPosition(program);
    program.setUniform("lightColor", color.x, color.y, color.z);
    program.setUniform("attenuationFactors",
        attenuationFactors.x, attenuationFactors.y, attenuationFactors.z);
}
