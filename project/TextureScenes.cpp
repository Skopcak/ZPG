#include "TextureScenes.h"
#include "Camera.h"
#include "plain_uv.h"
#include "door.h"
#include <glm/trigonometric.hpp>

TextureScenes::TextureScenes(Camera& camera)
    : camera(camera),
      fenceTexture("wooden_fence.png"),
      doorTexture("T_door_basecolor.png"),
      textureProgram("texture.vert", "texture.frag"),
      planeModel(plainUV, static_cast<GLsizei>(sizeof(plainUV) / (8 * sizeof(float))), 8),
      doorModel(doorVertices, doorVertexCount, 8),
      planeAspect(static_cast<float>(fenceTexture.getWidth()) / fenceTexture.getHeight(), 1.0f, 1.0f),
      planeOrientation(glm::radians(-90.0f), 0.0f, 0.0f, 1.0f),
      planeFacing(glm::radians(90.0f), 1.0f, 0.0f, 0.0f),
      planeObject(planeModel, textureProgram),
      doorObject(doorModel, textureProgram)
{
    textureProgram.setUniform("textureUnitID", 0);
    planeTransformation.add(planeAspect);
    planeTransformation.add(planeOrientation);
    planeTransformation.add(planeFacing);
    planeObject.setTransformation(planeTransformation);
    planeObject.getTransformation().scaleFactor = 2.0f;
    doorObject.getTransformation().scaleFactor = 2.2f;
    doorObject.getTransformation().angle = glm::radians(165.0f);
    planeScene.addObject(planeObject);
    doorScene.addObject(doorObject);
    camera.addObserver(textureProgram);
}

TextureScenes::~TextureScenes()
{
    camera.removeObserver(textureProgram);
}

void TextureScenes::bindTexture(const Scene& scene) const
{
    if (&scene == &planeScene)
        fenceTexture.bind();
    else if (&scene == &doorScene)
        doorTexture.bind();
}

void TextureScenes::unbindTexture(const Scene& scene) const
{
    if (&scene == &planeScene || &scene == &doorScene)
    {
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, 0);
    }
}
