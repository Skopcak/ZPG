#pragma once

#include "TransformationComponent.h"
#include <glm/vec3.hpp>

class Scale : public TransformationComponent
{
private:
    glm::vec3 factors;

public:
    Scale(float x, float y, float z);
    glm::mat4 getMatrix() const override;
};