#pragma once

#include <vector>
#include "DrawableObject.h"

class Scene
{
private:
    std::vector<DrawableObject*> objects;

public:
    void addObject(DrawableObject& object);
    void draw() const;
};