#include "SceneSelection.h"
#include "SceneBuilders.h"
#include "SceneLighting.h"
#include "TextureScenes.h"
#include <GLFW/glfw3.h>

SceneEntries createSceneEntries(const BasicScenes& basic, const ForestScenes& forests,
    const SolarScene& solar, const SceneView& clock, const LightingTestScenes& tests,
    TextureScenes& textures, const SceneLighting& lighting)
{
    return {{
        {GLFW_KEY_1, &basic.login.scene, &basic.login.controlledObject, &lighting.defaultLight},
        {GLFW_KEY_2, &basic.sphere.scene, &basic.sphere.controlledObject, &lighting.defaultLight},
        {GLFW_KEY_3, &basic.triangle.scene, &basic.triangle.controlledObject, &lighting.defaultLight},
        {GLFW_KEY_4, &forests.day.scene, &forests.day.controlledObject, &lighting.defaultLight},
        {GLFW_KEY_5, &solar.view.scene, &solar.view.controlledObject, &lighting.solarLight,
            &solar.view.controlledObject.getTransformation(), true},
        {GLFW_KEY_6, &clock.scene, &clock.controlledObject, &lighting.defaultLight},
        {GLFW_KEY_7, &tests.lambert.scene, &tests.lambert.controlledObject, &lighting.lightingTestLight},
        {GLFW_KEY_8, &tests.normals.scene, &tests.normals.controlledObject, &lighting.defaultLight},
        {GLFW_KEY_9, &tests.specular.scene, &tests.specular.controlledObject, &lighting.specularTestLight},
        {GLFW_KEY_0, &forests.night.scene, &forests.night.controlledObject, &lighting.nightLight,
            &forests.night.controlledObject.getTransformation()},
        {GLFW_KEY_F1, &textures.planeScene, &textures.planeObject, &lighting.defaultLight},
        {GLFW_KEY_F2, &textures.doorScene, &textures.doorObject, &lighting.defaultLight}
    }};
}

void selectScene(GLFWwindow* window, const SceneEntries& entries, const SceneEntry*& active)
{
    for (const auto& entry : entries)
        if (glfwGetKey(window, entry.key) == GLFW_PRESS)
            active = &entry;
}
