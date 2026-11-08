#include "CompositeTransformation.h"

void CompositeTransformation::add(
    Transformation& transformation)
{
    transformations.push_back(&transformation);
}

glm::mat4 CompositeTransformation::getMatrix() const
{
    glm::mat4 matrix(1.0f);

    // Combine matrices in the order of their insertion.
    for (const Transformation* transformation : transformations)
    {
        matrix = matrix * transformation->getMatrix();
    }

    return matrix;
}