#pragma once
#include "TransformationComponent.h"
#include <vector>

class CompositeTransformation : public TransformationComponent
{
private:
    std::vector<TransformationComponent*> transformations;

public:
    void add(TransformationComponent& transformation);
    glm::mat4 getMatrix() const override;
};