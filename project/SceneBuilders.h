#pragma once

#include "SolarAnimation.h"

class DrawableObject;
class Scene;
class SceneBuilder;

// Non-owning handles into SceneContent; never refer to builder-local storage.
struct SceneView
{
    Scene& scene;
    DrawableObject& controlledObject;
};

struct BasicScenes
{
    SceneView login;
    SceneView sphere;
    SceneView triangle;
    DrawableObject& signature;
};

BasicScenes buildBasicScenes(SceneBuilder& builder);

struct ForestScenes
{
    SceneView day;
    SceneView night;
};

ForestScenes buildForestScenes(SceneBuilder& builder, DrawableObject& signature);

SceneView buildClockScene(SceneBuilder& builder, Scene& forest);

struct LightingTestScenes
{
    SceneView lambert;
    SceneView normals;
    SceneView specular;
};

LightingTestScenes buildLightingTestScenes(SceneBuilder& builder);

struct SolarScene
{
    SceneView view;
    SolarRotations rotations;
};

SolarScene buildSolarScene(SceneBuilder& builder, DrawableObject& signature);
