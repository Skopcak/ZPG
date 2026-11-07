#pragma once

#include "Light.h"

class RenderResources;
class Transformation;
struct SceneEntry;

struct SceneLighting
{
    const Light defaultLight{.position = {10.0f, 10.0f, 10.0f}};
    const Light lightingTestLight;
    const Light specularTestLight{.position = {0.0f, 0.0f, -3.0f}};
    const Light solarLight;
    const Light nightLight;

    SceneLighting(const RenderResources& resources, const Transformation& firefly);
};

void applySceneLighting(const SceneEntry& entry, const RenderResources& resources);
