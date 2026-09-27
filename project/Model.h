#pragma once

#include <glad/gl.h>

class Model
{
private:
    GLuint VBO;
    GLuint VAO;
    GLsizei vertexCount;

public:
    // Jeden vrchol ma 6 hodnot: polohu a druhy atribut.
    Model(const float* vertices, GLsizei count);
    ~Model();

    void draw() const;

    Model(const Model&) = delete;
    Model& operator=(const Model&) = delete;
};