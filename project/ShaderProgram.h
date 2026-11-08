#pragma once

#include <glad/gl.h>
#include <glm/mat4x4.hpp>
#include "CameraObserver.h"


// Combines vertex and fragment shaders into a rendering program.
class ShaderProgram : public CameraObserver
{
private:
    GLuint programID;

public:

    ShaderProgram(const char* vertexFile, const char* fragmentFile);
    ~ShaderProgram() override;
    void setUniform(const char* name, float x, float y, float z) const;
    void setUniform(const char* name, float x, float y, float z, float w) const;
    void setUniform(const char* name, int value) const;
    void setUniform(const char* name, float value)const;
    void setUniform(
        const char* name,
        const glm::mat4& matrix
    ) const;

    void use() const;
    void onCameraChanged(const Camera& camera) override;

    ShaderProgram(const ShaderProgram&) = delete;
    ShaderProgram& operator=(const ShaderProgram&) = delete;

};
