#include "Scene.h"
#include <utility>

DrawableObject& Scene::addOwnedObject(DrawableObject object)
{
    // Presunieme vlastnictvo objektu do sceny. Jeho adresa sa uz nebude menit.
    ownedObjects.push_back(std::make_unique<DrawableObject>(std::move(object)));
    DrawableObject& storedObject = *ownedObjects.back();
    addObject(storedObject);
    return storedObject;
}

void Scene::addObject(DrawableObject& object){
    // Store object pointers without taking ownership.
    objects.push_back(&object);
}

void Scene::draw(const glm::mat4& viewMatrix) const{
    // Draw each object with its own shader and transformation.
    for (const DrawableObject* object : objects){
        object->draw(viewMatrix);
    }
}
void Scene::setShaderProgram(ShaderProgram& shaderProgram){
    for (DrawableObject* object : objects){
        object->setShaderProgram(shaderProgram);
    }
}
