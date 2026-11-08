#pragma once

#include "Scene.h"

// Pomaha vytvarat objekty. Ich zivotnost potom spravuje Scene.
class SceneFactory
{
private:
    Scene& scene;

public:
    explicit SceneFactory(Scene& scene);
    DrawableObject& addObject(Model& model, ShaderProgram& program);
    DrawableObject& addModel(const std::filesystem::path& path, ShaderProgram& program);
};
