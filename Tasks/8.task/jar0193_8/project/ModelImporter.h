#pragma once

#include <filesystem>
#include <string>
#include <vector>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <assimp/material.h>

// Data modelu v pamati CPU. Assimp ich nacita, OpenGL ich neskor vykresli.
struct ImportedTexture
{
    std::string name;
    std::filesystem::path path;
    // Pri externom PNG/JPG je data prazdne. GLB moze obsahovat obrazok priamo.
    std::vector<unsigned char> data;
};

struct ImportedMaterial
{
    glm::vec4 baseColor{1.0f};
    glm::vec3 specularColor{0.04f};
    glm::vec3 emissiveColor{0.0f};
    float shininess = 32.0f;
    bool transparent = false;
    // Index v ImportedModel::textures. -1 znamena, ze material nema texturu.
    int colorTexture = -1;
    int emissiveTexture = -1;
};

struct ImportedMesh
{
    // Jeden vrchol: poloha XYZ, farba RGBA, normala XYZ, UV (12 hodnot).
    std::vector<float> vertices;
    int materialIndex = 0;
    // Stred casti modelu, potrebny na zoradenie priehladnych casti.
    glm::vec3 center{0.0f};
};

struct ImportedModel
{
    std::vector<ImportedTexture> textures;
    std::vector<ImportedMaterial> materials;
    std::vector<ImportedMesh> meshes;
};

struct aiScene;
struct aiMesh;
struct aiMaterial;

class ModelImporter
{
public:
    static ImportedModel load(const std::filesystem::path& path);

private:
    static void importError(const std::filesystem::path& path, const std::string& message);
    static void checkStaticModel(const aiScene& scene, const std::filesystem::path& path);
    static int loadTexture(const aiMaterial& material, aiTextureType type,
        const aiScene& scene, const std::filesystem::path& path, ImportedModel& model);
    static void readColors(const aiMaterial& source, ImportedMaterial& material,
        const std::filesystem::path& path);
    static void readTransparency(const aiMaterial& source, ImportedMaterial& material,
        const std::filesystem::path& path);
    static void checkMaterial(const ImportedMaterial& material, const std::filesystem::path& path);
    static ImportedMaterial loadMaterial(const aiMaterial& source, const aiScene& scene,
        const std::filesystem::path& path, ImportedModel& model);
    static void appendVertex(ImportedMesh& part, const aiMesh& mesh, unsigned int index,
        const std::filesystem::path& path);
    static ImportedMesh loadMesh(const aiMesh& source, int materialIndex,
        const ImportedMaterial& material, const std::filesystem::path& path);
    static void loadParts(const aiScene& scene, const std::filesystem::path& path, ImportedModel& model);
};
