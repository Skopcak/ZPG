#pragma once

#include "Model.h"
#include "ShaderProgram.h"
#include "Transformation.h"
#include <vector>

class Texture;

class DrawableObject{
private:
    Model& model;
    Transformation transformation;
    ShaderProgram* shaderProgram;
    const Transformation* additionalTransformation = nullptr;
    struct TextureBinding{
        GLuint unit;
        const Texture* texture;
    };
    std::vector<TextureBinding> textures;

public:
    DrawableObject(Model& model, ShaderProgram& shaderProgram);
    void setShaderProgram(ShaderProgram& newShaderProgram);
    Transformation& getTransformation();
    void setTexture(const Texture& texture, GLuint unit = 0);

    void setTransformation(
        const Transformation& newTransformation);

    void draw() const;
};
