#include "Transformation.h"
#include <glm/gtc/matrix_transform.hpp>

glm::mat4 Transformation::getMatrix() const
{
    glm::mat4 matrix(1.0f);

    matrix = glm::translate(
        matrix,
        glm::vec3(offsetX, offsetY, offsetZ)
    );

    matrix = glm::rotate(
        matrix,
        angle,
        glm::vec3(0.0f, 1.0f, 0.0f)
    );

    matrix = glm::scale(
        matrix,
        glm::vec3(scaleFactor)
    );

    return matrix;
}