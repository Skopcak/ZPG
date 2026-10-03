#pragma once

#include <glad/gl.h>

class Model
{
private:
    GLuint VBO;
    GLuint VAO;
    GLsizei vertexCount;

public:
    Model(const float* vertices, GLsizei count);
    ~Model();

    void draw() const;

    Model(const Model&) = delete;
    Model& operator=(const Model&) = delete;
};