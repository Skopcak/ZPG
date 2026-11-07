#include "SceneBuilders.h"
#include "SceneBuilder.h"
#include "SceneContent.h"
#include "RenderResources.h"
#include "Scale.h"

LightingTestScenes buildLightingTestScenes(SceneBuilder& builder)
{
    auto& resources = builder.resources();
    auto& content = builder.content();
    auto& lambert = content.createScene();
    const glm::vec3 positions[] = {
        {0.0f, 0.8f, 0.0f}, {-0.8f, 0.0f, 0.0f},
        {0.8f, 0.0f, 0.0f}, {0.0f, -0.8f, 0.0f}
    };
    DrawableObject* firstSphere = nullptr;
    for (const auto& position : positions)
    {
        auto& object = builder.addSphere(lambert, resources.lambertProgram);
        auto& transform = object.getTransformation();
        transform.offsetX = position.x;
        transform.offsetY = position.y;
        transform.offsetZ = position.z;
        transform.scaleFactor = 0.35f;
        if (!firstSphere)
            firstSphere = &object;
    }

    auto& normals = content.createScene();
    auto& ellipsoid = builder.addSphere(normals, resources.normalTestProgram);
    ellipsoid.getTransformation().scaleFactor = 0.7f;
    ellipsoid.setTransformation(content.makeTransformation<Scale>(1.5f, 0.5f, 1.0f));
    resources.normalTestProgram.setUniform("normalMode", 2.0f);

    auto& specular = content.createScene();
    auto& sphere = builder.addSphere(specular, resources.specularTestProgram);
    sphere.getTransformation().scaleFactor = 0.7f;
    resources.specularTestProgram.setUniform("shininess", 1.0f);
    resources.specularTestProgram.setUniform("clipBackfaceSpecular", 1.0f);
    return {{lambert, *firstSphere}, {normals, ellipsoid}, {specular, sphere}};
}
