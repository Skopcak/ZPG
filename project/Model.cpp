#include "Model.h"

Model::Model(const float* vertices,GLsizei count,GLsizei valuesPerVertex){
    VBO = 0;
    VAO = 0;
    vertexCount = count;

    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    // Upload all values belonging to each vertex.
    glBufferData(
        GL_ARRAY_BUFFER,
        static_cast<GLsizeiptr>(vertexCount)
        * valuesPerVertex * sizeof(float),
        vertices,
        GL_STATIC_DRAW
    );

    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    const GLsizei stride = static_cast<GLsizei>(
        valuesPerVertex * sizeof(float));

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(
        0, 3, GL_FLOAT, GL_FALSE,
        stride, (void*)0
    );

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(
        1, 3, GL_FLOAT, GL_FALSE,
        stride, (void*)(3 * sizeof(float))
    );

    // Nine-value vertices also contain a separate normal.
    if (valuesPerVertex == 9)
    {
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(
            2, 3, GL_FLOAT, GL_FALSE,
            stride, (void*)(6 * sizeof(float))
        );
    }

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