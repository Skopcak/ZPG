#include "SceneBuilders.h"
#include "SceneBuilder.h"
#include "SceneContent.h"
#include "RenderResources.h"
#include "Scale.h"

SceneView buildClockScene(SceneBuilder& builder, Scene& forest)
{
    auto& resources = builder.resources();
    auto& content = builder.content();
    auto& scene = content.createScene();
    auto& clock = builder.addObject(scene, resources.clockMesh, resources.clockProgram);
    // All three clocks share the reflected face and clock-hand uniforms.
    auto& facing = content.makeTransformation<Scale>(1.0f, 1.0f, -1.0f);
    clock.setTransformation(facing);

    auto& first = builder.addObject(forest, resources.clockMesh, resources.clockProgram);
    first.setTransformation(facing);
    first.getTransformation().offsetX = -1.5f;
    first.getTransformation().offsetY = -0.25f;
    first.getTransformation().offsetZ = 0.0f;
    first.getTransformation().scaleFactor = 0.75f;

    auto& second = builder.addObject(forest, resources.clockMesh, resources.clockProgram);
    second.setTransformation(facing);
    second.getTransformation().offsetX = 1.5f;
    second.getTransformation().offsetY = -0.25f;
    second.getTransformation().offsetZ = -3.0f;
    second.getTransformation().scaleFactor = 0.75f;
    return {scene, clock};
}
