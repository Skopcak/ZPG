#pragma once

#include <array>
#include <vector>
#include <memory>
#include "DrawableObject.h"

class Scene
{
private:
    static constexpr unsigned int maxObjectId = 0xFF;
    // The slot index is the stencil ID. Slot 0 is reserved for the background.
    std::array<DrawableObject*, maxObjectId + 1> objects{};
    // Objekty vytvorene cez SceneFactory patria scene a zaniknu spolu s nou.
    std::vector<std::unique_ptr<DrawableObject>> ownedObjects;

    unsigned int findFreeObjectId() const;

public:
    unsigned int addObject(DrawableObject& object);
    DrawableObject& addOwnedObject(DrawableObject object);
    DrawableObject* getObjectById(unsigned int id) const;
    void draw(const glm::mat4& viewMatrix = glm::mat4(1.0f)) const;
    void setShaderProgram(ShaderProgram& shaderProgram);

    bool removeObject(unsigned int id);
};
