#include "DrawableObject.h"
#include "Texture.h"
#include <stdexcept>

DrawableObject::DrawableObject(Model& model,ShaderProgram& shaderProgram): model(model), shaderProgram(&shaderProgram){}
Transformation& DrawableObject::getTransformation(){
    return transformation;
}

void DrawableObject::setTransformation(const Transformation& newTransformation){
    additionalTransformation = &newTransformation;
}

void DrawableObject::draw() const{
    glm::mat4 modelMatrix = transformation.getMatrix();
    // Combine keyboard controls with the assigned transformation.
    if (additionalTransformation != nullptr)
    {
        modelMatrix =
            modelMatrix * additionalTransformation->getMatrix();
    }

    shaderProgram->use();
    shaderProgram->setUniform("modelMatrix", modelMatrix);

    if (textures.empty())
    {
        model.draw();
        return;
    }

    GLint previousActive = 0;
    glGetIntegerv(GL_ACTIVE_TEXTURE, &previousActive);
    std::vector<GLint> previousBindings(textures.size());
    for (std::size_t i = 0; i < textures.size(); ++i)
    {
        glActiveTexture(GL_TEXTURE0 + textures[i].unit);
        glGetIntegerv(GL_TEXTURE_BINDING_2D, &previousBindings[i]);
        textures[i].texture->bind(textures[i].unit);
    }

    model.draw();

    for (std::size_t i = 0; i < textures.size(); ++i)
    {
        glActiveTexture(GL_TEXTURE0 + textures[i].unit);
        glBindTexture(GL_TEXTURE_2D, previousBindings[i]);
    }
    glActiveTexture(previousActive);
}

void DrawableObject::setTexture(const Texture& texture, GLuint unit)
{
    GLint maxUnits = 0;
    glGetIntegerv(GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS, &maxUnits);
    if (unit >= static_cast<GLuint>(maxUnits))
    {
        throw std::out_of_range("Texture unit exceeds the OpenGL limit");
    }
    for (TextureBinding& binding : textures)
    {
        if (binding.unit == unit)
        {
            binding.texture = &texture;
            return;
        }
    }
    textures.push_back({unit, &texture});
}

void DrawableObject::setShaderProgram(
    ShaderProgram& newShaderProgram){
    shaderProgram = &newShaderProgram;
}
