#include "DrawableObject.h"

DrawableObject::DrawableObject(
    Model& model,
    ShaderProgram& shaderProgram)
    : model(model), shaderProgram(shaderProgram)
{}

Transformation& DrawableObject::getTransformation()
{   
    return transformation;
}

void DrawableObject::setTransformation(
    const Transformation& newTransformation)
{
    additionalTransformation = &newTransformation;
}

void DrawableObject::draw() const
{
    glm::mat4 modelMatrix = transformation.getMatrix();

    // Combine keyboard controls with the assigned transformation.
    if (additionalTransformation != nullptr)
    {
        modelMatrix =
            modelMatrix * additionalTransformation->getMatrix();
    }

    shaderProgram.use();
    shaderProgram.setUniform("modelMatrix", modelMatrix);

    model.draw();
}