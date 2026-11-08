#pragma once

#include "Model.h"
#include "ShaderProgram.h"
#include "Transformation.h"
#include "ModelImporter.h"
#include <memory>
#include <vector>

class Texture;

class DrawableObject{
private:
    Model* model = nullptr;
    Transformation transformation;
    ShaderProgram* shaderProgram;
    const Transformation* additionalTransformation = nullptr;
    struct TextureBinding{
        GLuint unit;
        const Texture* texture;
    };
    std::vector<TextureBinding> textures;
    struct MaterialBinding {
        ImportedMaterial properties;
        const Texture* colorTexture;
        const Texture* emissiveTexture;
    };
    struct MeshPart {
        std::unique_ptr<Model> model;
        int materialIndex;
        glm::vec3 center;
    };
    struct TransparentPart {
        const MeshPart* mesh;
        float depth;
    };
    std::vector<std::unique_ptr<Texture>> importedTextures;
    std::vector<MaterialBinding> importedMaterials;
    std::vector<MeshPart> importedMeshes;
    explicit DrawableObject(ShaderProgram& shaderProgram);
    const Texture* getImportedTexture(int index) const;
    void drawImportedPart(const MeshPart& part) const;
    static bool fartherFirst(const TransparentPart& left, const TransparentPart& right);
    void drawImported(const glm::mat4& modelMatrix, const glm::mat4& viewMatrix) const;

public:
    DrawableObject(Model& model, ShaderProgram& shaderProgram);
    ~DrawableObject();
    DrawableObject(const DrawableObject&) = delete;
    DrawableObject& operator=(const DrawableObject&) = delete;
    DrawableObject(DrawableObject&&) noexcept;
    DrawableObject& operator=(DrawableObject&&) noexcept;
    static DrawableObject load(const std::filesystem::path& path, ShaderProgram& shaderProgram);
    void setShaderProgram(ShaderProgram& newShaderProgram);
    Transformation& getTransformation();
    void setTexture(const Texture& texture, GLuint unit = 0);

    void setTransformation(
        const Transformation& newTransformation);

    void draw(const glm::mat4& viewMatrix = glm::mat4(1.0f)) const;
};
