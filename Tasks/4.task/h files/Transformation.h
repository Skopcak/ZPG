#pragma once
#include <glm/mat4x4.hpp>

class Transformation
{
public:
    virtual ~Transformation() = default;

    float offsetX = 0.0f;
    float offsetY = 0.0f;
    float offsetZ = 0.0f;

    float angle = 0.0f;
    float scaleFactor = 1.0f;

    virtual glm::mat4 getMatrix() const;
};