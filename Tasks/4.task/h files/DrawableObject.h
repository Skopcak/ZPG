#pragma once

#include "Model.h"
#include "ShaderProgram.h"
#include "Transformation.h"

class DrawableObject
{
private:
    Model& model;
    ShaderProgram& shaderProgram;
    Transformation transformation;

    const Transformation* additionalTransformation = nullptr;

public:
    DrawableObject(Model& model, ShaderProgram& shaderProgram);

    Transformation& getTransformation();

    void setTransformation(
        const Transformation& newTransformation);

    void draw() const;
};