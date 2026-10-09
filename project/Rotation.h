#pragma once

#include "TransformationComponent.h"
#include <glm/vec3.hpp>

class Rotation : public TransformationComponent
{
private:
    float angle;
    glm::vec3 axis;

public:
    Rotation(
        float angleInRadians,
        float axisX,
        float axisY,
        float axisZ
    );

    void setAngle(float angleInRadians);
    glm::mat4 getMatrix() const override;
};
