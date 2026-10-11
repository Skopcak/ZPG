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

public:
    DrawableObject(Model& model, ShaderProgram& shaderProgram);
    Transformation& getTransformation();

    void draw() const;
};