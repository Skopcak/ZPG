#pragma once

#include "Transformation.h"

class DrawableObject;
class Model;
class RenderResources;
class Scene;
class SceneContent;
class ShaderProgram;

class SceneBuilder
{
private:
    RenderResources& sharedResources;
    SceneContent& ownedContent;

public:
    SceneBuilder(RenderResources& resources, SceneContent& content);
    RenderResources& resources() const;
    SceneContent& content() const;

    DrawableObject& addObject(Scene& scene, Model& model, ShaderProgram& program,
        const Transformation& transform = {});
    DrawableObject& addSphere(Scene& scene, ShaderProgram& program,
        const Transformation& transform = {});
    DrawableObject& addTree(Scene& scene, const Transformation& transform, bool night = false);
    DrawableObject& addBush(Scene& scene, const Transformation& transform, bool night = false);
};
