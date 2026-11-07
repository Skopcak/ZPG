#include "SceneBuilders.h"
#include "SceneBuilder.h"
#include "SceneContent.h"
#include "RenderResources.h"
#include "Translation.h"
#include "Rotation.h"
#include "Scale.h"

BasicScenes buildBasicScenes(SceneBuilder& builder)
{
    auto& resources = builder.resources();
    auto& content = builder.content();
    auto& loginScene = content.createScene();
    auto& logo = builder.addObject(loginScene, resources.logoModel, resources.colorProgram);
    logo.getTransformation().angle = 0.5f;
    logo.getTransformation().scaleFactor = 0.8f;

    auto& sphereScene = content.createScene();
    auto& sphere = builder.addSphere(sphereScene, resources.blinnProgram);
    sphere.getTransformation().scaleFactor = 0.35f;

    auto& triangleScene = content.createScene();
    auto& triangle = builder.addObject(triangleScene, resources.triangleModel, resources.colorProgram);
    auto& translation = content.makeTransformation<Translation>(0.4f, 0.0f, 0.0f);
    auto& rotation = content.makeTransformation<Rotation>(0.7854f, 0.0f, 0.0f, 1.0f);
    auto& scale = content.makeTransformation<Scale>(0.6f, 0.6f, 0.6f);
    auto& movement = content.compose({&translation, &rotation});
    triangle.setTransformation(content.compose({&movement, &scale}));

    auto& signature = builder.addObject(sphereScene, resources.logoModel, resources.yellowProgram);
    auto& transform = signature.getTransformation();
    transform.offsetX = 0.70f;
    transform.offsetY = -0.90f;
    transform.offsetZ = -0.80f;
    transform.scaleFactor = 0.20f;
    triangleScene.addObject(signature);

    return {{loginScene, logo}, {sphereScene, sphere}, {triangleScene, triangle}, signature};
}
