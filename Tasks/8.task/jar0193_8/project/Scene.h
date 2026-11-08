#pragma once

#include <vector>
#include <memory>
#include "DrawableObject.h"

class Scene
{
private:
    std::vector<DrawableObject*> objects;
    // Objekty vytvorene cez SceneFactory patria scene a zaniknu spolu s nou.
    std::vector<std::unique_ptr<DrawableObject>> ownedObjects;

public:
    void addObject(DrawableObject& object);
    DrawableObject& addOwnedObject(DrawableObject object);
    void draw(const glm::mat4& viewMatrix = glm::mat4(1.0f)) const;
    void setShaderProgram(ShaderProgram& shaderProgram);
};
