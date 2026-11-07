#include "SceneContent.h"

Scene& SceneContent::createScene()
{
    auto scene = std::make_unique<Scene>();
    Scene& result = *scene;
    scenes.push_back(std::move(scene));
    return result;
}

DrawableObject& SceneContent::addObject(Scene& scene, Model& model, ShaderProgram& program)
{
    auto object = std::make_unique<DrawableObject>(model, program);
    DrawableObject& result = *object;
    objects.push_back(std::move(object));
    scene.addObject(result);
    return result;
}

CompositeTransformation& SceneContent::compose(std::initializer_list<Transformation*> parts)
{
    auto& composite = makeTransformation<CompositeTransformation>();
    for (Transformation* part : parts)
        composite.add(*part);
    return composite;
}
