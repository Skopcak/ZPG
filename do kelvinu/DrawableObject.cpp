#include "DrawableObject.h"

DrawableObject::DrawableObject(
    Model& model,
    ShaderProgram& shaderProgram)
    : model(model), shaderProgram(shaderProgram)
{}

void DrawableObject::draw() const
{
    shaderProgram.use();
    model.draw();
}