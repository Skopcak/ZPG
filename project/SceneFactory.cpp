#include "SceneFactory.h"

SceneFactory::SceneFactory(Scene& scene) : scene(scene) {}

DrawableObject& SceneFactory::addObject(Model& model, ShaderProgram& program)
{
    return scene.addOwnedObject(DrawableObject(model, program));
}

DrawableObject& SceneFactory::addModel(const std::filesystem::path& path, ShaderProgram& program)
{
    return scene.addOwnedObject(DrawableObject::load(path, program));
}
