#pragma once

#include "CompositeTransformation.h"
#include "Scene.h"
#include <initializer_list>
#include <memory>
#include <type_traits>
#include <utility>
#include <vector>

// Stable addresses for every object and every borrowed transformation.
// GPU resources and the camera outlive this owner.
class SceneContent
{
private:
    std::vector<std::unique_ptr<Transformation>> transformations;
    std::vector<std::unique_ptr<DrawableObject>> objects;
    std::vector<std::unique_ptr<Scene>> scenes;

public:
    SceneContent() = default;
    SceneContent(const SceneContent&) = delete;
    SceneContent& operator=(const SceneContent&) = delete;
    SceneContent(SceneContent&&) = delete;
    SceneContent& operator=(SceneContent&&) = delete;

    Scene& createScene();
    DrawableObject& addObject(Scene& scene, Model& model, ShaderProgram& program);
    CompositeTransformation& compose(std::initializer_list<Transformation*> parts);

    template<class T, class... Args>
    T& makeTransformation(Args&&... args)
    {
        static_assert(std::is_base_of_v<Transformation, T>);
        auto transformation = std::make_unique<T>(std::forward<Args>(args)...);
        T& result = *transformation;
        transformations.push_back(std::move(transformation));
        return result;
    }
};
