#include "Scene.h"

void Scene::addObject(DrawableObject& object)
{
    // Store object pointers without taking ownership.
    objects.push_back(&object);
}

void Scene::draw() const
{
    // Draw each object with its own shader and transformation.
    for (const DrawableObject* object : objects)
    {
        object->draw();
    }
}