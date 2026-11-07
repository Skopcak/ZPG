#pragma once

#include <glad/gl.h>

// Owns one 2D image on the GPU. The OpenGL context must outlive this object.
class Texture
{
private:
    GLuint id = 0;
    int width = 0;
    int height = 0;

public:
    explicit Texture(const char* filename,
        GLint wrapS = GL_REPEAT, GLint wrapT = GL_REPEAT);
    ~Texture();
    void bind(GLuint unit = 0) const;
    int getWidth() const { return width; }
    int getHeight() const { return height; }

    Texture(const Texture&) = delete;
    Texture& operator=(const Texture&) = delete;
};
