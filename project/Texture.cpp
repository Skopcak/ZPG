#include "Texture.h"
#include "stb_image.h"

#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>

Texture::Texture(const char* filename, GLint wrapS, GLint wrapT)
{
    int channels = 0;
    stbi_set_flip_vertically_on_load(true);
    std::unique_ptr<stbi_uc, decltype(&stbi_image_free)> pixels(
        stbi_load(filename, &width, &height, &channels, 4), stbi_image_free);
    if (!pixels)
    {
        throw std::runtime_error(std::string("Failed to load ") + filename
            + ": " + stbi_failure_reason());
    }

    GLint previousActive = 0, previousBinding = 0;
    glGetIntegerv(GL_ACTIVE_TEXTURE, &previousActive);
    glActiveTexture(GL_TEXTURE0);
    glGetIntegerv(GL_TEXTURE_BINDING_2D, &previousBinding);
    glGenTextures(1, &id);
    glBindTexture(GL_TEXTURE_2D, id);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0,
        GL_RGBA, GL_UNSIGNED_BYTE, pixels.get());
    glGenerateMipmap(GL_TEXTURE_2D);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrapS);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrapT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, previousBinding);
    glActiveTexture(previousActive);

    std::cout << "Loaded " << filename << ": " << width << " x " << height
              << " (RGBA)\n";
}

Texture::~Texture()
{
    glDeleteTextures(1, &id);
}

void Texture::bind(GLuint unit) const
{
    glActiveTexture(GL_TEXTURE0 + unit);
    glBindTexture(GL_TEXTURE_2D, id);
}
