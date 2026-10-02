#include "Model.h"

Model::Model(const float* vertices, GLsizei count)
    : VBO(0), VAO(0), vertexCount(count)
{
    // Vytvorenie buffera a nahratie vrcholov.
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    glBufferData(
        GL_ARRAY_BUFFER,
        static_cast<GLsizeiptr>(vertexCount) * 6 * sizeof(float),
        vertices,
        GL_STATIC_DRAW
    );

    // Nastavenie sposobu citania vrcholov.
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    // Prve tri hodnoty: poloha.
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(
        0, 3, GL_FLOAT, GL_FALSE,
        6 * sizeof(float), (void*)0
    );

    // Dalsie tri hodnoty: druhy atribut.
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(
        1, 3, GL_FLOAT, GL_FALSE,
        6 * sizeof(float), (void*)(3 * sizeof(float))
    );

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void Model::draw() const
{
    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLES, 0, vertexCount);
}

Model::~Model()
{
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
}