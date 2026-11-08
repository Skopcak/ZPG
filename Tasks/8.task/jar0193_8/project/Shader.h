#pragma once

#include <glad/gl.h>

// Stores one compiled vertex or fragment shader.
class Shader
{
private:
    GLuint shaderID;

public:
    Shader(GLenum shaderType, const char* shaderFile);
    ~Shader();

    GLuint getID() const;

    Shader(const Shader&) = delete;
    Shader& operator=(const Shader&) = delete;
};
