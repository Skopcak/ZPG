#pragma once

#include "CompositeTransformation.h"
#include "DrawableObject.h"
#include "Rotation.h"
#include "Scale.h"
#include "Scene.h"
#include "Texture.h"

class Camera;

// Owns the resources of the existing F1/F2 scenes at stable addresses.
class TextureScenes
{
private:
    Camera& camera;
    Texture fenceTexture;
    Texture doorTexture;
    ShaderProgram textureProgram;
    Model planeModel;
    Model doorModel;
    Scale planeAspect;
    Rotation planeOrientation;
    Rotation planeFacing;
    CompositeTransformation planeTransformation;

public:
    DrawableObject planeObject;
    DrawableObject doorObject;
    Scene planeScene;
    Scene doorScene;

    explicit TextureScenes(Camera& camera);
    ~TextureScenes();
    void bindTexture(const Scene& scene) const;
    void unbindTexture(const Scene& scene) const;
};
