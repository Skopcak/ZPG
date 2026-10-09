#include "Scale.h"
#include <glm/gtc/matrix_transform.hpp>

Scale::Scale(float x, float y, float z)
{
    factors = glm::vec3(x, y, z);
}

glm::mat4 Scale::getMatrix() const
{
    glm::mat4 matrix(1.0f);
    matrix = glm::scale(matrix, factors);

    return matrix;
}