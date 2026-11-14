#pragma once

struct GLFWwindow;
class Camera;
class DrawableObject;

// Borrows the window and camera; keeps mouse state across scene changes.
class InputController
{
public:
    InputController(GLFWwindow* window, Camera& camera);
    void update(DrawableObject* activeObject, float deltaTime);

private:
    void updateObject(DrawableObject& object, float deltaTime) const;
    void updateCamera(float deltaTime);

    GLFWwindow* window;
    Camera& camera;
    double previousMouseX = 0.0;
    double previousMouseY = 0.0;
    bool wasLooking = false;
};
