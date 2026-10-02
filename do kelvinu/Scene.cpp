#include "Scene.h"

void Scene::addObject(DrawableObject& object)
{
    objects.push_back(&object);
}

void Scene::draw() const
{
    for (const DrawableObject* object : objects)
    {
        object->draw();
    }
}