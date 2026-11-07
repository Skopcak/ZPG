#include "SceneBuilders.h"
#include "SceneBuilder.h"
#include "SceneContent.h"
#include "RenderResources.h"
#include "Translation.h"
#include "Rotation.h"
#include "Scale.h"

#include <glm/trigonometric.hpp>

SolarScene buildSolarScene(SceneBuilder& builder, DrawableObject& signature)
{
    auto& resources = builder.resources();
    auto& content = builder.content();
    Scene& solarScene = content.createScene();


    auto& solarSunObject = builder.addObject(solarScene, resources.solarSunModel, resources.ambientProgram);
    auto& earthObject = builder.addObject(solarScene, resources.earthModel, resources.phongProgram);
    auto& moonObject = builder.addObject(solarScene, resources.moonModel, resources.blinnProgram);
    auto& venatorObject = builder.addObject(solarScene, resources.venatorModel, resources.phongProgram);
    auto& imperialObject = builder.addObject(solarScene, resources.imperialModel, resources.blinnProgram);

    auto& solarSunSpin = content.makeTransformation<Rotation>(0.0f, 0.0f, 1.0f, 0.0f);
    auto& solarSunScale = content.makeTransformation<Scale>(0.18f, 0.18f, 0.18f);
    auto& solarSunTransformation = content.makeTransformation<CompositeTransformation>();
    solarSunTransformation.add(solarSunSpin);
    solarSunTransformation.add(solarSunScale);
    solarSunObject.setTransformation(solarSunTransformation);

    // Keep Earth's orbital position separate from its size.
    auto& earthDistance = content.makeTransformation<Translation>(0.55f, 0.0f, 0.0f);
    auto& earthOrbitRotation = content.makeTransformation<Rotation>(0.0f, 0.0f, 0.0f, 1.0f);

    auto& earthOrbit = content.makeTransformation<CompositeTransformation>();
    earthOrbit.add(earthOrbitRotation);
    earthOrbit.add(earthDistance);

    auto& earthSpin = content.makeTransformation<Rotation>(0.0f, 1.0f, 0.0f, 0.0f);
    auto& earthScale = content.makeTransformation<Scale>(0.10f, 0.10f, 0.10f);

    auto& earthTransformation = content.makeTransformation<CompositeTransformation>();
    earthTransformation.add(solarSunObject.getTransformation());
    earthTransformation.add(earthOrbit);
    earthTransformation.add(earthSpin);
    earthTransformation.add(earthScale);
    earthObject.setTransformation(earthTransformation);

    // Position the Moon relative to Earth's orbital position.
    auto& moonDistance = content.makeTransformation<Translation>(0.20f, 0.0f, 0.0f);
    auto& moonOrbitRotation = content.makeTransformation<Rotation>(0.0f, 0.0f, 0.0f, 1.0f);

    auto& moonOrbit = content.makeTransformation<CompositeTransformation>();
    moonOrbit.add(moonOrbitRotation);
    moonOrbit.add(moonDistance);

    auto& moonSpin = content.makeTransformation<Rotation>(0.0f, 1.0f, 0.0f, 0.0f);
    auto& moonScale = content.makeTransformation<Scale>(0.045f, 0.045f, 0.045f);

    auto& moonTransformation = content.makeTransformation<CompositeTransformation>();
    moonTransformation.add(solarSunObject.getTransformation());
    moonTransformation.add(earthOrbit);
    moonTransformation.add(moonOrbit);
    moonTransformation.add(moonSpin);
    moonTransformation.add(moonScale);
    moonObject.setTransformation(moonTransformation);

    // Orbit around the vertical Y axis, moving toward and away from the camera.
    auto& venatorOrbitRotation = content.makeTransformation<Rotation>(0.0f, 0.0f, 1.0f, 0.0f);
    auto& imperialOrbitRotation = content.makeTransformation<Rotation>(0.0f, 0.0f, 1.0f, 0.0f);
    auto& venatorDistance = content.makeTransformation<Translation>(0.0f, 0.0f, 1.05f);
    auto& imperialDistance = content.makeTransformation<Translation>(0.0f, 0.0f, 1.40f);
    auto& imperialFacing = content.makeTransformation<Rotation>(glm::radians(180.0f), 0.0f, 0.0f, 1.0f);
    auto& shipTilt = content.makeTransformation<Rotation>(glm::radians(65.0f), 1.0f, 0.0f, 0.0f);
    auto& shipScale = content.makeTransformation<Scale>(0.28f, 0.28f, 0.28f);

    // Rotate the orbital offset before applying the ship's local tilt and size.
    auto& venatorTransformation = content.makeTransformation<CompositeTransformation>();
    venatorTransformation.add(solarSunObject.getTransformation());
    venatorTransformation.add(venatorOrbitRotation);
    venatorTransformation.add(venatorDistance);
    venatorTransformation.add(shipTilt);
    venatorTransformation.add(shipScale);
    venatorObject.setTransformation(venatorTransformation);

    auto& imperialTransformation = content.makeTransformation<CompositeTransformation>();
    imperialTransformation.add(solarSunObject.getTransformation());
    imperialTransformation.add(imperialOrbitRotation);
    imperialTransformation.add(imperialDistance);
    imperialTransformation.add(imperialFacing);
    imperialTransformation.add(shipTilt);
    imperialTransformation.add(shipScale);
    imperialObject.setTransformation(imperialTransformation);

    solarScene.addObject(signature);



    return {{solarScene, solarSunObject}, {earthOrbitRotation, moonOrbitRotation,
        earthSpin, moonSpin, solarSunSpin, venatorOrbitRotation, imperialOrbitRotation}};
}
