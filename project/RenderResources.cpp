#include "RenderResources.h"
#include "Camera.h"

#include "Sphere.h"
#include "tree.h"
#include "bushes.h"
#include "Login.h"
#include "sun.h"
#include "earth.h"
#include "moon.h"
#include "venator.h"
#include "imperial.h"
#include "plain.h"
#include "clock.h"

namespace
{
    constexpr float triangleVertices[] = {
         0.0f,  0.5f, 0.0f,    1.0f, 0.0f, 0.0f,
        -0.5f, -0.5f, 0.0f,    0.0f, 1.0f, 0.0f,
         0.5f, -0.5f, 0.0f,    0.0f, 0.0f, 1.0f
    };

    template<int Stride, size_t N>
    constexpr GLsizei meshVertexCount(const float (&)[N])
    {
        return static_cast<GLsizei>(N / Stride);
    }
}

RenderResources::RenderResources(Camera& camera)
    : camera(camera),
      colorProgram("shaders/basic.vert", "shaders/basic.frag"),
      yellowProgram("shaders/basic.vert", "shaders/yellow.frag"),
      treeProgram("tree.vert", "shaders/basic.frag"),
      clockProgram("clock.vert", "shaders/basic.frag"),
      bushProgram("bush.vert", "shaders/basic.frag"),
      phongProgram("phong.vert", "phong.frag"),
      blinnProgram("phong.vert", "blinn.frag"),
      ambientProgram("shaders/basic.vert", "ambient.frag"),
      lambertProgram("phong.vert", "lambert.frag"),
      normalTestProgram("normal_test.vert", "phong.frag"),
      specularTestProgram("phong.vert", "specular_test.frag"),
      nightTreeProgram("tree.vert", "night.frag"),
      nightBushProgram("bush.vert", "night.frag"),
      nightGroundProgram("phong.vert", "night.frag"),
      logoModel(loginVertices, meshVertexCount<6>(loginVertices)),
      sphereModel(sphere, meshVertexCount<6>(sphere)),
      triangleModel(triangleVertices, meshVertexCount<6>(triangleVertices)),
      treeModel(tree, meshVertexCount<6>(tree)),
      groundModel(plain, 6),
      bushModel(bushes, meshVertexCount<6>(bushes)),
      solarSunModel(sun, meshVertexCount<9>(sun), 9),
      earthModel(earth, meshVertexCount<9>(earth), 9),
      moonModel(moon, meshVertexCount<9>(moon), 9),
      venatorModel(venatorVertices, venatorVertexCount, 9),
      imperialModel(imperialVertices, imperialVertexCount, 9),
      clockMesh(clockModel, meshVertexCount<9>(clockModel), 9)
{
    for (ShaderProgram* program : cameraPrograms)
        camera.addObserver(*program);
}

RenderResources::~RenderResources()
{
    for (ShaderProgram* program : cameraPrograms)
        camera.removeObserver(*program);
}
