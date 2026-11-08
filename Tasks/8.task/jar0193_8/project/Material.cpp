#include "Material.h"
#include "ShaderProgram.h"

void Material::apply(const ShaderProgram& program) const
{
    program.setUniform("materialColor", color.x, color.y, color.z);
    program.setUniform("useVertexColor", useVertexColor ? 1.0f : 0.0f);
    program.setUniform("shininess", shininess);
    program.setUniform("specularStrength", specularStrength);
}
