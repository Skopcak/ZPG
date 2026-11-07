#include "SceneBuilders.h"
#include "SceneBuilder.h"
#include "SceneContent.h"
#include "RenderResources.h"
#include "Material.h"

#include <random>
#include <vector>

namespace
{
    Transformation nightPlacement(const Transformation& day)
    {
        Transformation transform = day;
        // Leave a clearing between the initial camera and the firefly.
        if (transform.offsetX > -2.0f && transform.offsetX < 2.0f
            && transform.offsetZ > -2.0f && transform.offsetZ < 5.0f)
            transform.offsetX += transform.offsetX < 0.0f ? -4.0f : 4.0f;
        return transform;
    }
}

ForestScenes buildForestScenes(SceneBuilder& builder, DrawableObject& signature)
{
    auto& resources = builder.resources();
    auto& content = builder.content();
    auto& day = content.createScene();
    auto& ground = builder.addObject(day, resources.groundModel, resources.colorProgram);
    ground.getTransformation().scaleFactor = 10.0f;
    ground.getTransformation().offsetY = -1.0f;

    std::mt19937 forestGenerator(std::random_device{}());
    std::uniform_real_distribution<float> forestX(-8.0f, 8.0f);
    std::uniform_real_distribution<float> forestZ(-8.0f, 8.0f);
    std::uniform_real_distribution<float> forestAngle(0.0f, 6.283185f);
    std::uniform_real_distribution<float> treeScaleRange(0.25f, 0.45f);
    std::uniform_real_distribution<float> bushScaleRange(1.0f, 2.0f);

    std::vector<DrawableObject*> trees;
    std::vector<DrawableObject*> bushes;
    for (int i = 0; i < 74; ++i)
    {
        Transformation transform;
        transform.offsetX = forestX(forestGenerator);
        transform.offsetY = -1.0f;
        transform.offsetZ = forestZ(forestGenerator);
        transform.scaleFactor = treeScaleRange(forestGenerator);
        transform.angle = forestAngle(forestGenerator);
        trees.push_back(&builder.addTree(day, transform));
    }
    for (int i = 0; i < 25; ++i)
    {
        Transformation transform;
        transform.offsetX = forestX(forestGenerator);
        transform.offsetY = -1.0f;
        transform.offsetZ = forestZ(forestGenerator);
        transform.scaleFactor = bushScaleRange(forestGenerator);
        transform.angle = forestAngle(forestGenerator);
        bushes.push_back(&builder.addBush(day, transform));
    }

    // Night instances share the same GPU meshes and copy the daytime placement.
    auto& night = content.createScene();
    builder.addObject(night, resources.groundModel, resources.nightGroundProgram,
        ground.getTransformation());
    for (DrawableObject* tree : trees)
        builder.addTree(night, nightPlacement(tree->getTransformation()), true);
    for (DrawableObject* bush : bushes)
        builder.addBush(night, nightPlacement(bush->getTransformation()), true);

    const Material vegetation{.useVertexColor = true};
    const Material soil{.color = {0.18f, 0.12f, 0.06f}};
    vegetation.apply(resources.nightTreeProgram);
    vegetation.apply(resources.nightBushProgram);
    soil.apply(resources.nightGroundProgram);

    auto& firefly = builder.addSphere(night, resources.yellowProgram);
    firefly.getTransformation().offsetY = -0.3f;
    firefly.getTransformation().scaleFactor = 0.05f;

    auto& sun = builder.addSphere(day, resources.yellowProgram);
    auto& sunTransform = sun.getTransformation();
    sunTransform.offsetX = 0.75f;
    sunTransform.offsetY = 0.78f;
    sunTransform.offsetZ = 0.5f;
    sunTransform.scaleFactor = 0.12f;
    day.addObject(signature);

    return {{day, *trees.front()}, {night, firefly}};
}
