#pragma once

class Camera;

class CameraObserver
{
public:
    virtual ~CameraObserver() = default;

    virtual void onCameraChanged(const Camera& camera) = 0;
};