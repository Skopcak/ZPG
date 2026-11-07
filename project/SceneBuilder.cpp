#include "SceneBuilder.h"
#include "RenderResources.h"
#include "SceneContent.h"

SceneBuilder::SceneBuilder(RenderResources& resources, SceneContent& content)
    : sharedResources(resources), ownedContent(content)
{
}

RenderResources& SceneBuilder::resources() const { return sharedResources; }
SceneContent& SceneBuilder::content() const { return ownedContent; }

DrawableObject& SceneBuilder::addObject(Scene& scene, Model& model,
    ShaderProgram& program, const Transformation& transform)
{
    auto& object = ownedContent.addObject(scene, model, program);
    object.getTransformation() = transform;
    return object;
}

DrawableObject& SceneBuilder::addSphere(Scene& scene, ShaderProgram& program,
    const Transformation& transform)
{
    return addObject(scene, sharedResources.sphereModel, program, transform);
}

DrawableObject& SceneBuilder::addTree(Scene& scene, const Transformation& transform, bool night)
{
    return addObject(scene, sharedResources.treeModel,
        night ? sharedResources.nightTreeProgram : sharedResources.treeProgram, transform);
}

DrawableObject& SceneBuilder::addBush(Scene& scene, const Transformation& transform, bool night)
{
    return addObject(scene, sharedResources.bushModel,
        night ? sharedResources.nightBushProgram : sharedResources.bushProgram, transform);
}
