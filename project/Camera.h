#pragma once

#include <glm/mat4x4.hpp>
#include <glm/vec3.hpp>
#include <vector>


class CameraObserver;

class Camera
{
private:
    glm::mat4 viewMatrix;
    glm::mat4 projectionMatrix;
    int projectionWidth = 0;
    int projectionHeight = 0;

    glm::vec3 eye;
    glm::vec3 target;
    glm::vec3 up;
    float alpha;
    float fi;
    void updateView();

    std::vector<CameraObserver*> observers;

    void notifyObservers();


public:
    Camera();
    glm::mat4 getCamera() const;
    glm::mat4 getProjectionMatrix() const;


    void updateProjection(int width, int height);
    void moveForward(float distance);
    void moveRight(float distance);
    void rotate(float deltaAlpha, float deltaFi);

    void addObserver(CameraObserver& observer);
    void removeObserver(CameraObserver& observer);


    glm::vec3 getPosition() const;


};
