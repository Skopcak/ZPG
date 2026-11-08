#include "DrawableObject.h"
#include "Texture.h"
#include <algorithm>
#include <limits>
#include <stdexcept>

DrawableObject::DrawableObject(Model& model,ShaderProgram& shaderProgram): model(&model), shaderProgram(&shaderProgram){}
DrawableObject::DrawableObject(ShaderProgram& shaderProgram): shaderProgram(&shaderProgram){}
DrawableObject::~DrawableObject() = default;
DrawableObject::DrawableObject(DrawableObject&&) noexcept = default;
DrawableObject& DrawableObject::operator=(DrawableObject&&) noexcept = default;

DrawableObject DrawableObject::load(const std::filesystem::path& path, ShaderProgram& shaderProgram)
{
    const ImportedModel source = ModelImporter::load(path);
    DrawableObject object(shaderProgram);
    for (const ImportedTexture& texture : source.textures) {
        if (texture.data.empty()) {
            object.importedTextures.push_back(std::make_unique<Texture>(
                texture.path.string().c_str(), GL_REPEAT, GL_REPEAT, false));
        } else {
            object.importedTextures.push_back(std::make_unique<Texture>(texture.data, false));
        }
    }
    // Biely pixel zastupi material, ktory nema vlastny obrazok.
    object.importedTextures.push_back(std::make_unique<Texture>(std::array<unsigned char, 4>{255, 255, 255, 255}));
    for (const ImportedMaterial& material : source.materials) {
        MaterialBinding binding;
        binding.properties = material;
        binding.colorTexture = object.getImportedTexture(material.colorTexture);
        binding.emissiveTexture = object.getImportedTexture(material.emissiveTexture);
        object.importedMaterials.push_back(binding);
    }
    for (const ImportedMesh& mesh : source.meshes) {
        if (mesh.vertices.empty() || mesh.vertices.size() % 36 != 0
            || mesh.vertices.size() / 12 > static_cast<std::size_t>(std::numeric_limits<GLsizei>::max())
            || mesh.materialIndex < 0 || mesh.materialIndex >= object.importedMaterials.size()) {
            throw std::runtime_error("Invalid imported mesh: " + path.string());
        }
        object.importedMeshes.push_back({std::make_unique<Model>(mesh.vertices.data(),
            static_cast<GLsizei>(mesh.vertices.size() / 12), 12), mesh.materialIndex, mesh.center});
    }
    shaderProgram.setUniform("colorTexture", 0);
    shaderProgram.setUniform("emissiveTexture", 1);
    return object;
}

const Texture* DrawableObject::getImportedTexture(int index) const
{
    if (index == -1) {
        return importedTextures.back().get();
    }
    return importedTextures.at(index).get();
}
Transformation& DrawableObject::getTransformation(){
    return transformation;
}

void DrawableObject::setTransformation(const Transformation& newTransformation){
    additionalTransformation = &newTransformation;
}

void DrawableObject::draw(const glm::mat4& viewMatrix) const{
    glm::mat4 modelMatrix = transformation.getMatrix();
    // Combine keyboard controls with the assigned transformation.
    if (additionalTransformation != nullptr)
    {
        modelMatrix =
            modelMatrix * additionalTransformation->getMatrix();
    }

    shaderProgram->use();
    shaderProgram->setUniform("modelMatrix", modelMatrix);

    if (!importedMeshes.empty()) {
        drawImported(modelMatrix, viewMatrix);
        return;
    }

    if (textures.empty())
    {
        model->draw();
        return;
    }

    GLint previousActive = 0;
    glGetIntegerv(GL_ACTIVE_TEXTURE, &previousActive);
    std::vector<GLint> previousBindings(textures.size());
    for (std::size_t i = 0; i < textures.size(); ++i)
    {
        glActiveTexture(GL_TEXTURE0 + textures[i].unit);
        glGetIntegerv(GL_TEXTURE_BINDING_2D, &previousBindings[i]);
        textures[i].texture->bind(textures[i].unit);
    }

    model->draw();

    for (std::size_t i = 0; i < textures.size(); ++i)
    {
        glActiveTexture(GL_TEXTURE0 + textures[i].unit);
        glBindTexture(GL_TEXTURE_2D, previousBindings[i]);
    }
    glActiveTexture(previousActive);
}

void DrawableObject::setTexture(const Texture& texture, GLuint unit)
{
    GLint maxUnits = 0;
    glGetIntegerv(GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS, &maxUnits);
    if (unit >= static_cast<GLuint>(maxUnits))
    {
        throw std::out_of_range("Texture unit exceeds the OpenGL limit");
    }
    for (TextureBinding& binding : textures)
    {
        if (binding.unit == unit)
        {
            binding.texture = &texture;
            return;
        }
    }
    textures.push_back({unit, &texture});
}

void DrawableObject::setShaderProgram(
    ShaderProgram& newShaderProgram){
    shaderProgram = &newShaderProgram;
}

void DrawableObject::drawImported(const glm::mat4& modelMatrix, const glm::mat4& viewMatrix) const
{
    std::vector<GLuint> units{0, 1};
    for (const TextureBinding& binding : textures)
        if (std::find(units.begin(), units.end(), binding.unit) == units.end()) units.push_back(binding.unit);
    std::vector<GLint> previousBindings(units.size());
    GLint previousActive = 0, sourceRGB = 0, destinationRGB = 0, sourceAlpha = 0, destinationAlpha = 0;
    GLint equationRGB = 0, equationAlpha = 0;
    GLboolean previousMask = GL_TRUE;
    const GLboolean previousBlend = glIsEnabled(GL_BLEND), previousCull = glIsEnabled(GL_CULL_FACE);
    glGetIntegerv(GL_ACTIVE_TEXTURE, &previousActive);
    glGetBooleanv(GL_DEPTH_WRITEMASK, &previousMask);
    glGetIntegerv(GL_BLEND_SRC_RGB, &sourceRGB);
    glGetIntegerv(GL_BLEND_DST_RGB, &destinationRGB);
    glGetIntegerv(GL_BLEND_SRC_ALPHA, &sourceAlpha);
    glGetIntegerv(GL_BLEND_DST_ALPHA, &destinationAlpha);
    glGetIntegerv(GL_BLEND_EQUATION_RGB, &equationRGB);
    glGetIntegerv(GL_BLEND_EQUATION_ALPHA, &equationAlpha);
    for (std::size_t i = 0; i < units.size(); ++i) {
        glActiveTexture(GL_TEXTURE0 + units[i]);
        glGetIntegerv(GL_TEXTURE_BINDING_2D, &previousBindings[i]);
    }
    glDisable(GL_CULL_FACE);
    glDisable(GL_BLEND);

    // Najskor nakreslime pevne casti; priehladne si odlozime na koniec.
    const glm::mat4 modelView = viewMatrix * modelMatrix;
    std::vector<TransparentPart> transparent;
    for (const MeshPart& part : importedMeshes) {
        if (importedMaterials[part.materialIndex].properties.transparent) {
            float depth = (modelView * glm::vec4(part.center, 1.0f)).z;
            transparent.push_back({&part, depth});
        } else {
            drawImportedPart(part);
        }
    }
    if (!transparent.empty()) {
        std::stable_sort(transparent.begin(), transparent.end(), fartherFirst);
        glEnable(GL_BLEND);
        glBlendEquation(GL_FUNC_ADD);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glDepthMask(GL_FALSE);
        for (const TransparentPart& part : transparent) {
            drawImportedPart(*part.mesh);
        }
    }

    glDepthMask(previousMask);
    glBlendFuncSeparate(sourceRGB, destinationRGB, sourceAlpha, destinationAlpha);
    glBlendEquationSeparate(equationRGB, equationAlpha);
    if (previousBlend) glEnable(GL_BLEND); else glDisable(GL_BLEND);
    if (previousCull) glEnable(GL_CULL_FACE); else glDisable(GL_CULL_FACE);
    for (std::size_t i = 0; i < units.size(); ++i) {
        glActiveTexture(GL_TEXTURE0 + units[i]);
        glBindTexture(GL_TEXTURE_2D, previousBindings[i]);
    }
    glActiveTexture(previousActive);
}

bool DrawableObject::fartherFirst(const TransparentPart& left, const TransparentPart& right)
{
    return left.depth < right.depth;
}

void DrawableObject::drawImportedPart(const MeshPart& part) const
{
    const MaterialBinding& binding = importedMaterials[part.materialIndex];
    const ImportedMaterial& material = binding.properties;
    const glm::vec4& color = material.baseColor;
    shaderProgram->setUniform("baseColorFactor", color.r, color.g, color.b, color.a);
    shaderProgram->setUniform("specularColor", material.specularColor.r, material.specularColor.g, material.specularColor.b);
    shaderProgram->setUniform("emissiveColor", material.emissiveColor.r, material.emissiveColor.g, material.emissiveColor.b);
    shaderProgram->setUniform("shininess", material.shininess);
    int alphaMode = 0;
    if (material.transparent) {
        alphaMode = 2;
    }
    shaderProgram->setUniform("alphaMode", alphaMode);
    binding.colorTexture->bind(0);
    binding.emissiveTexture->bind(1);
    for (const TextureBinding& overrideTexture : textures) {
        overrideTexture.texture->bind(overrideTexture.unit);
    }
    part.model->draw();
}
