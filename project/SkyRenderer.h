#pragma once

#include "ShaderProgram.h"
#include "Texture.h"

class Camera;

class SkyRenderer
{
public:
    enum class Mode { None, Cube, Dome };
    explicit SkyRenderer(Camera& camera);
    ~SkyRenderer();
    void draw(Mode mode) const;

    SkyRenderer(const SkyRenderer&) = delete;
    SkyRenderer& operator=(const SkyRenderer&) = delete;

private:
    Camera& camera;
    ShaderProgram cubeProgram;
    ShaderProgram domeProgram;
    Texture domeTexture;
    GLuint cubeTexture = 0;
    GLuint arrays[2]{};
    GLuint buffers[2]{};
    GLsizei domeVertexCount = 0;
    void release();
};
