#include "Scene.h"
#include <stdexcept>
#include <utility>
#include <algorithm>

DrawableObject& Scene::addOwnedObject(DrawableObject object)
{
    const unsigned int id = findFreeObjectId();
    // Presunieme vlastnictvo objektu do sceny. Jeho adresa sa uz nebude menit.
    ownedObjects.push_back(std::make_unique<DrawableObject>(std::move(object)));
    objects[id] = ownedObjects.back().get();
    return *objects[id];
}

unsigned int Scene::findFreeObjectId() const
{
    for (unsigned int id = 1; id <= maxObjectId; ++id) {
        if (objects[id] == nullptr) {
            return id;
        }
    }
    throw std::overflow_error("Scene has reached the limit of 255 object IDs");
}

unsigned int Scene::addObject(DrawableObject& object)
{
    const unsigned int id = findFreeObjectId();
    objects[id] = &object;
    return id;
}

DrawableObject* Scene::getObjectById(unsigned int id) const
{
    if (id > maxObjectId) {
        return nullptr;
    }
    return objects[id];
}

void Scene::draw(const glm::mat4& viewMatrix) const {
    for (unsigned int id = 1; id <= maxObjectId; ++id){
        const DrawableObject* object = objects[id];
        if (object != nullptr){
                glStencilFunc(GL_ALWAYS, static_cast<GLint>(id), 0xFF);
                object->draw(viewMatrix);
        }
    }
}

void Scene::setShaderProgram(ShaderProgram& shaderProgram){
    for (DrawableObject* object : objects){
        if (object != nullptr) {
            object->setShaderProgram(shaderProgram);
        }
    }
}

bool Scene::removeObject(unsigned int id)
{
    DrawableObject* object = getObjectById(id);
    if (object == nullptr) {
        return false;
    }

    const auto owner = std::find_if(
        ownedObjects.begin(), ownedObjects.end(),
        [object](const std::unique_ptr<DrawableObject>& candidate) {
            return candidate.get() == object;
        });

    objects[id] = nullptr;
    if (owner != ownedObjects.end()) {
        std::replace(objects.begin(), objects.end(), object, static_cast<DrawableObject*>(nullptr));
        ownedObjects.erase(owner);
    }
    return true;
}
