#include "Rotation.h"
#include <glm/gtc/matrix_transform.hpp>

Rotation::Rotation(
    float angleInRadians,
    float axisX,
    float axisY,
    float axisZ)
{
    angle = angleInRadians;
    axis = glm::vec3(axisX, axisY, axisZ);
}

void Rotation::setAngle(float angleInRadians)
{
    angle = angleInRadians;
}

glm::mat4 Rotation::getMatrix() const
{
    glm::mat4 matrix(1.0f);
    matrix = glm::rotate(matrix, angle, axis);

    return matrix;
}