#pragma once

#include <glad/gl.h>

// Combines vertex and fragment shaders into a rendering program.
class ShaderProgram
{
private:
    GLuint programID;

public:

    ShaderProgram(const char* vertexFile, const char* fragmentFile);
    ~ShaderProgram();
    void setUniform(const char* name, float x, float y, float z) const;
    void setUniform(const char* name, float value)const;

    void use() const;

    ShaderProgram(const ShaderProgram&) = delete;
    ShaderProgram& operator=(const ShaderProgram&) = delete;
};
