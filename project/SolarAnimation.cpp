#include "SolarAnimation.h"
#include "Rotation.h"

#include <glm/trigonometric.hpp>

SolarAnimation::SolarAnimation(SolarRotations rotations)
    : rotations(rotations),
      venatorOrbitAngle(glm::radians(-45.0f)),
      imperialOrbitAngle(glm::radians(45.0f))
{
}

void SolarAnimation::update(float deltaTime)
{
    earthOrbitAngle += 0.2f * deltaTime;
    moonOrbitAngle += 1.2f * deltaTime;
    rotations.earthOrbit.setAngle(earthOrbitAngle);
    rotations.moonOrbit.setAngle(moonOrbitAngle);

    constexpr float shipOrbitSpeed = 0.35f;
    venatorOrbitAngle += shipOrbitSpeed * deltaTime;
    imperialOrbitAngle -= shipOrbitSpeed * deltaTime;
    rotations.venatorOrbit.setAngle(venatorOrbitAngle);
    rotations.imperialOrbit.setAngle(imperialOrbitAngle);

    earthSpinAngle += 0.8f * deltaTime;
    moonSpinAngle += 0.6f * deltaTime;
    rotations.earthSpin.setAngle(earthSpinAngle);
    rotations.moonSpin.setAngle(moonSpinAngle);

    sunSpinAngle += 0.4f * deltaTime;
    rotations.sunSpin.setAngle(sunSpinAngle);
}
