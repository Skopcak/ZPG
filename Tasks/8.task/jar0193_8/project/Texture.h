#pragma once

#include <glad/gl.h>
#include <array>
#include <span>

// Owns one 2D image on the GPU. The OpenGL context must outlive this object.
class Texture
{
private:
    GLuint id = 0;
    int width = 0;
    int height = 0;
    void upload(const unsigned char* pixels, GLint wrapS, GLint wrapT);

public:
    explicit Texture(const char* filename,
        GLint wrapS = GL_REPEAT, GLint wrapT = GL_REPEAT, bool flipVertically = true);
    explicit Texture(std::span<const unsigned char> encoded, bool flipVertically = true);
    explicit Texture(const std::array<unsigned char, 4>& color);
    ~Texture();
    void bind(GLuint unit = 0) const;
    int getWidth() const { return width; }
    int getHeight() const { return height; }

    Texture(const Texture&) = delete;
    Texture& operator=(const Texture&) = delete;
};
