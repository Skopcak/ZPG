#include "DrawableObject.h"

DrawableObject::DrawableObject(
    Model& model,
    ShaderProgram& shaderProgram)
    : model(model), shaderProgram(shaderProgram)
{}

void DrawableObject::draw() const
{
    glm::mat4 modelMatrix = transformation.getMatrix();

    shaderProgram.use();
    shaderProgram.setUniform("modelMatrix", modelMatrix);

    model.draw();
}

Transformation& DrawableObject::getTransformation()
{
    return transformation;
}