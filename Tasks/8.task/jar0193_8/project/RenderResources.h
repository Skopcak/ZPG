#pragma once

#include "Model.h"
#include "ShaderProgram.h"
#include <array>

class Camera;

// Shared GPU models and programs; scene objects are created in Application::run().
class RenderResources
{
private:
    Camera& camera;

public:
    ShaderProgram colorProgram;
    ShaderProgram yellowProgram;
    ShaderProgram treeProgram;
    ShaderProgram clockProgram;
    ShaderProgram bushProgram;
    ShaderProgram phongProgram;
    ShaderProgram blinnProgram;
    ShaderProgram ambientProgram;
    ShaderProgram lambertProgram;
    ShaderProgram normalTestProgram;
    ShaderProgram specularTestProgram;
    ShaderProgram nightTreeProgram;
    ShaderProgram nightBushProgram;
    ShaderProgram nightGroundProgram;

    Model logoModel;
    Model sphereModel;
    Model triangleModel;
    Model treeModel;
    Model groundModel;
    Model bushModel;
    Model solarSunModel;
    Model earthModel;
    Model moonModel;
    Model venatorModel;
    Model imperialModel;
    Model clockMesh;

    explicit RenderResources(Camera& camera);
    ~RenderResources();
    RenderResources(const RenderResources&) = delete;
    RenderResources& operator=(const RenderResources&) = delete;
    RenderResources(RenderResources&&) = delete;
    RenderResources& operator=(RenderResources&&) = delete;

private:
    const std::array<ShaderProgram*, 14> cameraPrograms{
        &colorProgram, &yellowProgram, &treeProgram, &clockProgram,
        &bushProgram, &phongProgram, &blinnProgram, &ambientProgram,
        &lambertProgram, &normalTestProgram, &specularTestProgram,
        &nightTreeProgram, &nightBushProgram, &nightGroundProgram
    };
};
