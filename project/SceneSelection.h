#pragma once

#include <array>

class DrawableObject;
class Scene;
class TextureScenes;
class Transformation;
struct BasicScenes;
struct ForestScenes;
struct Light;
struct LightingTestScenes;
struct SceneLighting;
struct SceneView;
struct SolarScene;
struct GLFWwindow;

struct SceneEntry
{
    int key;
    Scene* scene;
    DrawableObject* controlledObject;
    const Light* light;
    const Transformation* lightSource = nullptr;
    bool useVertexColor = false;
};

using SceneEntries = std::array<SceneEntry, 12>;

SceneEntries createSceneEntries(const BasicScenes& basic, const ForestScenes& forests,
    const SolarScene& solar, const SceneView& clock, const LightingTestScenes& tests,
    TextureScenes& textures, const SceneLighting& lighting);

// Last pressed entry wins, preserving the original 1..9, 0, F1/F2 priority.
void selectScene(GLFWwindow* window, const SceneEntries& entries, const SceneEntry*& active);
