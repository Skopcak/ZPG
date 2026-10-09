#pragma once

#include "Model.h"
#include "ShaderProgram.h"
#include "Transformation.h"
#include "TransformationComponent.h"

class DrawableObject
{
private:
    Model& model;
    ShaderProgram& shaderProgram;
    Transformation transformation;

    const TransformationComponent* additionalTransformation = nullptr;

public:
    DrawableObject(Model& model, ShaderProgram& shaderProgram);

    Transformation& getTransformation();

    void setTransformation(
        const TransformationComponent& newTransformation);

    void draw() const;
};