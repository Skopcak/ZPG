#include "SkyRenderer.h"
#include "Camera.h"
#include "stb_image.h"

#include <array>
#include <cmath>
#include <memory>
#include <stdexcept>
#include <string>
#include <vector>

static void uploadSkyMesh(GLuint& array, GLuint& buffer,
    const std::vector<float>& vertices, GLsizei valuesPerVertex)
{
    glGenVertexArrays(1, &array);
    glGenBuffers(1, &buffer);
    glBindVertexArray(array);
    glBindBuffer(GL_ARRAY_BUFFER, buffer);
    glBufferData(GL_ARRAY_BUFFER, vertices.size()*sizeof(float), vertices.data(), GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, valuesPerVertex*sizeof(float), nullptr);
    if (valuesPerVertex == 5)
    {
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE,
            5*sizeof(float), reinterpret_cast<void*>(3*sizeof(float)));
    }
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

SkyRenderer::SkyRenderer(Camera& camera)
    : camera(camera),
      cubeProgram("skybox.vert", "skybox.frag"),
      domeProgram("skydome.vert", "skydome.frag"),
      domeTexture("Textures/sky_panorama.png", GL_REPEAT, GL_CLAMP_TO_EDGE)
{
    try
    {
        const char* faces[] = {"posx", "negx", "posy", "negy", "posz", "negz"};
        GLint previousActive = 0, previousBinding = 0;
        glGetIntegerv(GL_ACTIVE_TEXTURE, &previousActive);
        glActiveTexture(GL_TEXTURE2);
        glGetIntegerv(GL_TEXTURE_BINDING_CUBE_MAP, &previousBinding);
        glGenTextures(1, &cubeTexture);
        glBindTexture(GL_TEXTURE_CUBE_MAP, cubeTexture);
        int faceSize = 0;
        for (int i = 0; i < 6; ++i)
        {
            const std::string path = std::string("Textures/skybox/") + faces[i] + ".jpg";
            int width = 0, height = 0, channels = 0;
            // The teacher's images use the OpenGL cube face orientation, without a 2D UV flip.
            stbi_set_flip_vertically_on_load(false);
            std::unique_ptr<stbi_uc, decltype(&stbi_image_free)> pixels(
                stbi_load(path.c_str(), &width, &height, &channels, 4), stbi_image_free);
            if (!pixels || width != height || (i > 0 && width != faceSize))
            {
                glBindTexture(GL_TEXTURE_CUBE_MAP, previousBinding);
                glActiveTexture(previousActive);
                throw std::runtime_error("Cannot load matching square cubemap face: " + path);
            }
            faceSize = width;
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X+i, 0, GL_RGBA8,
                width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels.get());
        }
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
        glBindTexture(GL_TEXTURE_CUBE_MAP, previousBinding);
        glActiveTexture(previousActive);
        stbi_set_flip_vertically_on_load(true);
        glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);

        const float corners[6][4][3] = {
            {{1,-1,-1},{1,-1,1},{1,1,1},{1,1,-1}},
            {{-1,-1,1},{-1,-1,-1},{-1,1,-1},{-1,1,1}},
            {{-1,1,-1},{1,1,-1},{1,1,1},{-1,1,1}},
            {{-1,-1,1},{1,-1,1},{1,-1,-1},{-1,-1,-1}},
            {{1,-1,1},{-1,-1,1},{-1,1,1},{1,1,1}},
            {{-1,-1,-1},{1,-1,-1},{1,1,-1},{-1,1,-1}}
        };
        std::vector<float> cube;
        for (const auto& face : corners)
            for (int corner : {0,1,2,0,2,3})
                cube.insert(cube.end(), face[corner], face[corner]+3);
        uploadSkyMesh(arrays[0], buffers[0], cube, 3);

        constexpr int slices = 96, stacks = 48;
        constexpr float pi = 3.14159265358979323846f;
        std::vector<float> dome;
        dome.reserve(slices*stacks*6*5);
        auto append = [&](float u, float v)
        {
            const float longitude = (u-0.5f)*2*pi;
            const float latitude = (v-0.5f)*pi;
            dome.insert(dome.end(), {std::cos(latitude)*std::cos(longitude),
                std::sin(latitude), std::cos(latitude)*std::sin(longitude), u, v});
        };
        for (int row = 0; row < stacks; ++row)
            for (int column = 0; column < slices; ++column)
            {
                const float u0 = static_cast<float>(column)/slices;
                const float u1 = static_cast<float>(column+1)/slices;
                const float v0 = static_cast<float>(row)/stacks;
                const float v1 = static_cast<float>(row+1)/stacks;
                append(u0,v0); append(u1,v0); append(u1,v1);
                append(u0,v0); append(u1,v1); append(u0,v1);
            }
        domeVertexCount = static_cast<GLsizei>(dome.size()/5);
        uploadSkyMesh(arrays[1], buffers[1], dome, 5);
        cubeProgram.setUniform("skyTexture", 2);
        domeProgram.setUniform("skyTexture", 2);
        camera.addObserver(cubeProgram);
        camera.addObserver(domeProgram);
    }
    catch (...)
    {
        release();
        throw;
    }
}

SkyRenderer::~SkyRenderer()
{
    camera.removeObserver(cubeProgram);
    camera.removeObserver(domeProgram);
    release();
}

void SkyRenderer::release()
{
    glDeleteVertexArrays(2, arrays);
    glDeleteBuffers(2, buffers);
    glDeleteTextures(1, &cubeTexture);
}

void SkyRenderer::draw(Mode mode) const
{
    if (mode == Mode::None) return;
    GLint previousDepth = 0, previousActive = 0, previousArray = 0, previousProgram = 0;
    GLint previous2D = 0, previousCube = 0;
    GLboolean previousMask = GL_TRUE;
    const GLboolean previousCull = glIsEnabled(GL_CULL_FACE);
    glGetIntegerv(GL_DEPTH_FUNC, &previousDepth);
    glGetBooleanv(GL_DEPTH_WRITEMASK, &previousMask);
    glGetIntegerv(GL_ACTIVE_TEXTURE, &previousActive);
    glGetIntegerv(GL_VERTEX_ARRAY_BINDING, &previousArray);
    glGetIntegerv(GL_CURRENT_PROGRAM, &previousProgram);
    glActiveTexture(GL_TEXTURE2);
    glGetIntegerv(GL_TEXTURE_BINDING_2D, &previous2D);
    glGetIntegerv(GL_TEXTURE_BINDING_CUBE_MAP, &previousCube);
    glDepthMask(GL_FALSE);
    glDepthFunc(GL_LEQUAL);
    glDisable(GL_CULL_FACE);

    if (mode == Mode::Cube)
    {
        cubeProgram.use();
        glBindTexture(GL_TEXTURE_CUBE_MAP, cubeTexture);
        glBindVertexArray(arrays[0]);
        glDrawArrays(GL_TRIANGLES, 0, 36);
    }
    else
    {
        domeProgram.use();
        domeTexture.bind(2);
        glBindVertexArray(arrays[1]);
        glDrawArrays(GL_TRIANGLES, 0, domeVertexCount);
    }

    glBindTexture(GL_TEXTURE_2D, previous2D);
    glBindTexture(GL_TEXTURE_CUBE_MAP, previousCube);
    glActiveTexture(previousActive);
    glBindVertexArray(previousArray);
    glUseProgram(previousProgram);
    glDepthMask(previousMask);
    glDepthFunc(previousDepth);
    if (previousCull) glEnable(GL_CULL_FACE);
}
