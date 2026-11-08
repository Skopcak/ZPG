#include "ModelImporter.h"

#include <assimp/GltfMaterial.h>
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <cmath>
#include <cstring>
#include <limits>
#include <stdexcept>

ImportedModel ModelImporter::load(const std::filesystem::path& path)
{
    // 1. Assimp otvori subor a rozdeli plochy na trojuholniky.
    Assimp::Importer importer;
    unsigned int flags = aiProcess_Triangulate | aiProcess_GenSmoothNormals
        | aiProcess_ValidateDataStructure | aiProcess_FlipUVs;
    const aiScene* scene = importer.ReadFile(path.string(), flags);
    if (scene == nullptr) {
        importError(path, importer.GetErrorString());
    }
    checkStaticModel(*scene, path);

    // 2. Zapocitame polohy, rotacie a mierky objektov z Blenderu do vrcholov.
    scene = importer.ApplyPostProcessing(aiProcess_PreTransformVertices | aiProcess_GenBoundingBoxes);
    if (scene == nullptr) {
        importError(path, importer.GetErrorString());
    }
    if (scene->mRootNode == nullptr || !scene->HasMeshes()) {
        importError(path, "no triangle meshes found");
    }

    // 3. Skopirujeme materialy, textury a vrcholy do nasich vlastnych poli.
    ImportedModel model;
    loadParts(*scene, path, model);
    return model;
}

void ModelImporter::loadParts(const aiScene& scene, const std::filesystem::path& path, ImportedModel& model)
{
    // -1 znamena, ze tento material este nebol nacitany.
    std::vector<int> materialIndices(scene.mNumMaterials, -1);
    for (unsigned int i = 0; i < scene.mNumMeshes; i++) {
        const aiMesh& mesh = *scene.mMeshes[i];
        unsigned int sourceIndex = mesh.mMaterialIndex;
        if (sourceIndex >= scene.mNumMaterials || scene.mMaterials[sourceIndex] == nullptr) {
            importError(path, "invalid material index");
        }

        // Viac casti modelu moze pouzivat rovnaky material.
        if (materialIndices[sourceIndex] == -1) {
            materialIndices[sourceIndex] = static_cast<int>(model.materials.size());
            model.materials.push_back(loadMaterial(*scene.mMaterials[sourceIndex], scene, path, model));
        }
        int materialIndex = materialIndices[sourceIndex];
        model.meshes.push_back(loadMesh(mesh, materialIndex, model.materials[materialIndex], path));
    }
}

ImportedMaterial ModelImporter::loadMaterial(const aiMaterial& source, const aiScene& scene,
    const std::filesystem::path& path, ImportedModel& model)
{
    ImportedMaterial material;
    readColors(source, material, path);
    readTransparency(source, material, path);
    checkMaterial(material, path);

    aiTextureType colorType = aiTextureType_DIFFUSE;
    if (source.GetTextureCount(aiTextureType_BASE_COLOR) > 0) {
        colorType = aiTextureType_BASE_COLOR;
    }
    material.colorTexture = loadTexture(source, colorType, scene, path, model);
    material.emissiveTexture = loadTexture(source, aiTextureType_EMISSIVE, scene, path, model);
    return material;
}

int ModelImporter::loadTexture(const aiMaterial& material, aiTextureType type,
    const aiScene& scene, const std::filesystem::path& path, ImportedModel& model)
{
    if (material.GetTextureCount(type) == 0) {
        return -1; // Material tento druh textury nema.
    }

    aiString name;
    unsigned int channel = 0;
    if (material.GetTexture(type, 0, &name, nullptr, &channel) != AI_SUCCESS) {
        importError(path, "cannot read texture reference");
    }
    if (channel != 0) {
        importError(path, "only UV channel 0 is supported");
    }

    // Obrazok zdielany viacerymi materialmi staci ulozit raz.
    for (std::size_t i = 0; i < model.textures.size(); i++) {
        if (model.textures[i].name == name.C_Str()) {
            return static_cast<int>(i);
        }
    }

    ImportedTexture texture;
    texture.name = name.C_Str();
    const aiTexture* embedded = scene.GetEmbeddedTexture(name.C_Str());
    if (embedded == nullptr) {
        texture.path = (path.parent_path() / texture.name).lexically_normal();
    } else {
        if (embedded->mHeight != 0 || embedded->mWidth == 0 || embedded->pcData == nullptr) {
            importError(path, "embedded textures must be encoded PNG/JPEG images");
        }
        // GLB obsahuje PNG/JPG priamo v subore. Bajty si musime skopirovat.
        texture.data.resize(embedded->mWidth);
        std::memcpy(texture.data.data(), embedded->pcData, texture.data.size());
    }

    model.textures.push_back(texture);
    return static_cast<int>(model.textures.size()) - 1;
}

void ModelImporter::readColors(const aiMaterial& source, ImportedMaterial& material,
    const std::filesystem::path& path)
{
    aiColor4D color(1.0f, 1.0f, 1.0f, 1.0f);
    if (source.Get(AI_MATKEY_COLOR_DIFFUSE, color) != AI_SUCCESS) {
        source.Get(AI_MATKEY_BASE_COLOR, color);
    }
    material.baseColor = {color.r, color.g, color.b, color.a};

    aiColor3D specular;
    if (source.Get(AI_MATKEY_COLOR_SPECULAR, specular) == AI_SUCCESS) {
        material.specularColor = {specular.r, specular.g, specular.b};
    } else {
        // glTF moze miesto farby odlesku udavat metalickost povrchu.
        float metallic = 0.0f;
        source.Get(AI_MATKEY_METALLIC_FACTOR, metallic);
        if (!std::isfinite(metallic) || metallic < 0.0f || metallic > 1.0f) {
            importError(path, "invalid metallic factor");
        }
        material.specularColor = glm::vec3(0.04f) * (1.0f - metallic)
            + glm::vec3(material.baseColor) * metallic;
    }

    aiColor3D emission(0.0f, 0.0f, 0.0f);
    source.Get(AI_MATKEY_COLOR_EMISSIVE, emission);
    material.emissiveColor = {emission.r, emission.g, emission.b};
    source.Get(AI_MATKEY_SHININESS, material.shininess);
}

void ModelImporter::readTransparency(const aiMaterial& source, ImportedMaterial& material,
    const std::filesystem::path& path)
{
    aiString alpha;
    if (source.Get(AI_MATKEY_GLTF_ALPHAMODE, alpha) == AI_SUCCESS) {
        std::string mode = alpha.C_Str();
        if (mode == "BLEND") {
            material.transparent = true;
        } else if (mode != "OPAQUE") {
            importError(path, "unsupported alpha mode: " + mode);
        }
    } else {
        float opacity = 1.0f;
        source.Get(AI_MATKEY_OPACITY, opacity);
        if (!std::isfinite(opacity) || opacity < 0.0f || opacity > 1.0f) {
            importError(path, "invalid opacity");
        }
        if (opacity < 1.0f) {
            material.baseColor.a = opacity;
            material.transparent = true;
        }
    }
}

ImportedMesh ModelImporter::loadMesh(const aiMesh& source, int materialIndex,
    const ImportedMaterial& material, const std::filesystem::path& path)
{
    if (!source.HasPositions() || !source.HasNormals() || source.mNumFaces == 0) {
        importError(path, "mesh is missing positions, normals or faces");
    }
    if (source.mNumFaces > static_cast<unsigned int>(std::numeric_limits<int>::max() / 3)) {
        importError(path, "mesh is too large for an OpenGL draw call");
    }
    if (!source.HasTextureCoords(0) && (material.colorTexture != -1 || material.emissiveTexture != -1)) {
        importError(path, "textured mesh has no UV coordinates");
    }

    ImportedMesh part;
    part.materialIndex = materialIndex;
    // Kazdy trojuholnik ma 3 vrcholy a kazdy vrchol ma 12 hodnot.
    part.vertices.reserve(static_cast<std::size_t>(source.mNumFaces) * 3 * 12);
    for (unsigned int i = 0; i < source.mNumFaces; i++) {
        const aiFace& triangle = source.mFaces[i];
        if (triangle.mNumIndices != 3 || triangle.mIndices == nullptr) {
            importError(path, "mesh contains a non-triangle face");
        }
        for (unsigned int corner = 0; corner < 3; corner++) {
            appendVertex(part, source, triangle.mIndices[corner], path);
        }
    }

    aiVector3D center = (source.mAABB.mMin + source.mAABB.mMax) * 0.5f;
    if (!std::isfinite(center.x) || !std::isfinite(center.y) || !std::isfinite(center.z)) {
        importError(path, "invalid mesh bounds");
    }
    part.center = {center.x, center.y, center.z};
    return part;
}

void ModelImporter::appendVertex(ImportedMesh& part, const aiMesh& mesh, unsigned int index,
    const std::filesystem::path& path)
{
    if (index >= mesh.mNumVertices) {
        importError(path, "invalid vertex index");
    }
    aiVector3D position = mesh.mVertices[index];
    aiVector3D normal = mesh.mNormals[index];
    aiColor4D color(1.0f, 1.0f, 1.0f, 1.0f);
    if (mesh.HasVertexColors(0)) {
        color = mesh.mColors[0][index];
    }
    aiVector3D uv(0.0f, 0.0f, 0.0f);
    if (mesh.HasTextureCoords(0)) {
        uv = mesh.mTextureCoords[0][index];
    }

    float values[] = {
        position.x, position.y, position.z,
        color.r, color.g, color.b, color.a,
        normal.x, normal.y, normal.z,
        uv.x, uv.y
    };
    for (float value : values) {
        if (!std::isfinite(value)) {
            importError(path, "nonfinite vertex data");
        }
        part.vertices.push_back(value);
    }
}

void ModelImporter::checkStaticModel(const aiScene& scene, const std::filesystem::path& path)
{
    if (scene.HasAnimations()) {
        importError(path, "only static models are supported");
    }
    for (unsigned int i = 0; i < scene.mNumMeshes; i++) {
        if (scene.mMeshes[i]->HasBones()) {
            importError(path, "skinned meshes are not supported");
        }
    }
}

void ModelImporter::checkMaterial(const ImportedMaterial& material, const std::filesystem::path& path)
{
    for (int i = 0; i < 4; i++) {
        float color = material.baseColor[i];
        if (!std::isfinite(color) || color < 0.0f || color > 1.0f) {
            importError(path, "invalid material base color");
        }
    }
    for (int i = 0; i < 3; i++) {
        float specular = material.specularColor[i];
        float emissive = material.emissiveColor[i];
        if (!std::isfinite(specular) || specular < 0.0f
            || !std::isfinite(emissive) || emissive < 0.0f) {
            importError(path, "invalid specular or emissive color");
        }
    }
    if (!std::isfinite(material.shininess) || material.shininess < 0.0f) {
        importError(path, "invalid shininess");
    }
}

void ModelImporter::importError(const std::filesystem::path& path, const std::string& message)
{
    throw std::runtime_error("Cannot import " + path.string() + ": " + message);
}
