#pragma once

#include "Model.h"
#include "ShaderProgram.h"

class DrawableObject
{
private:
    Model& model;
    ShaderProgram& shaderProgram;

public:
    DrawableObject(Model& model, ShaderProgram& shaderProgram);

    void draw() const;
};