#pragma once

class Rotation;

// Borrowed rotations must outlive the animation.
struct SolarRotations
{
    Rotation& earthOrbit;
    Rotation& moonOrbit;
    Rotation& earthSpin;
    Rotation& moonSpin;
    Rotation& sunSpin;
    Rotation& venatorOrbit;
    Rotation& imperialOrbit;
};

class SolarAnimation
{
private:
    SolarRotations rotations;
    float earthOrbitAngle = 0.0f;
    float moonOrbitAngle = 0.0f;
    float earthSpinAngle = 0.0f;
    float moonSpinAngle = 0.0f;
    float sunSpinAngle = 0.0f;
    float venatorOrbitAngle;
    float imperialOrbitAngle;

public:
    explicit SolarAnimation(SolarRotations rotations);
    void update(float deltaTime);
};
