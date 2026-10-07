#pragma once

#include "TransformationComponent.h"
#include <glm/vec3.hpp>

class Translation : public TransformationComponent
{
private:
    glm::vec3 offset;

public:
    Translation(float x, float y, float z);
    glm::mat4 getMatrix() const override;
};