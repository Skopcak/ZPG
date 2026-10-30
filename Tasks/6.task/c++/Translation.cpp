#include "Translation.h"
#include <glm/gtc/matrix_transform.hpp>

Translation::Translation(float x, float y, float z)
{
    offset = glm::vec3(x, y, z);
}

glm::mat4 Translation::getMatrix() const
{
    glm::mat4 matrix(1.0f);
    matrix = glm::translate(matrix, offset);

    return matrix;
}