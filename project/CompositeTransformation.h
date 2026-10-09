#pragma once
#include "Transformation.h"
#include <vector>

class CompositeTransformation : public Transformation
{
private:
    std::vector<Transformation*> transformations;

public:
    void add(Transformation& transformation);
    glm::mat4 getMatrix() const override;
};