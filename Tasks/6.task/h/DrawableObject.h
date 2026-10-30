#pragma once

#include "Model.h"
#include "ShaderProgram.h"
#include "Transformation.h"

class DrawableObject
{
private:
    Model& model;
    Transformation transformation;
    ShaderProgram* shaderProgram;
    const Transformation* additionalTransformation = nullptr;

public:
    DrawableObject(Model& model, ShaderProgram& shaderProgram);
    void setShaderProgram(ShaderProgram& newShaderProgram);
    Transformation& getTransformation();

    void setTransformation(
        const Transformation& newTransformation);

    void draw() const;
};