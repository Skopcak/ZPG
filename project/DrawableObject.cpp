#include "DrawableObject.h"

DrawableObject::DrawableObject(
    Model& model,
    ShaderProgram& shaderProgram)
    : model(model), shaderProgram(shaderProgram)
{}

void DrawableObject::draw() const
{

    // Send this object's transformation before drawing its shared model.
    shaderProgram.use();
    shaderProgram.setUniform(
        "offset",
        transformation.offsetX,
        transformation.offsetY,
        transformation.offsetZ
    );

    shaderProgram.setUniform("angle", transformation.angle);
    shaderProgram.setUniform(
        "scaleFactor", transformation.scaleFactor);

    model.draw();
}

Transformation& DrawableObject::getTransformation()
{
    return transformation;
}