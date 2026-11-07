#include "SceneLighting.h"
#include "SceneSelection.h"
#include "RenderResources.h"
#include "Transformation.h"

namespace
{
    glm::vec3 positionOf(const Transformation& transform)
    {
        return {transform.offsetX, transform.offsetY, transform.offsetZ};
    }
}

SceneLighting::SceneLighting(const RenderResources& resources, const Transformation& firefly)
    : nightLight{.position = positionOf(firefly),
        .color = {2.0f, 1.8f, 0.8f}, .attenuationFactors = {1.0f, 0.7f, 1.8f}}
{
    nightLight.apply(resources.nightTreeProgram);
    nightLight.apply(resources.nightBushProgram);
    nightLight.apply(resources.nightGroundProgram);
}

void applySceneLighting(const SceneEntry& entry, const RenderResources& resources)
{
    Light light = *entry.light;
    if (entry.lightSource)
        light.position = positionOf(*entry.lightSource);

    const ShaderProgram* programs[] = {
        &resources.lambertProgram, &resources.phongProgram, &resources.blinnProgram,
        &resources.normalTestProgram, &resources.specularTestProgram,
        &resources.nightTreeProgram, &resources.nightBushProgram, &resources.nightGroundProgram
    };
    for (const ShaderProgram* program : programs)
        light.applyPosition(*program);

    const float useVertexColor = entry.useVertexColor ? 1.0f : 0.0f;
    resources.ambientProgram.setUniform("useVertexColor", useVertexColor);
    resources.phongProgram.setUniform("useVertexColor", useVertexColor);
    resources.blinnProgram.setUniform("useVertexColor", useVertexColor);
}
