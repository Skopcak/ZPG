#pragma once

#include <glm/mat4x4.hpp>
#include <glm/vec3.hpp>


class ShaderProgram;

class Camera
{
private:
    ShaderProgram* m_shaderProgram;
    glm::mat4 viewMatrix;
    glm::mat4 projectionMatrix;

    glm::vec3 eye;
    glm::vec3 target;
    glm::vec3 up;
    float alpha;
    float fi;


public:
    Camera(ShaderProgram* shaderProgram);
    glm::mat4 getCamera();
    glm::mat4 getProjectionMatrix() const;

    void updateProjection(int width, int height);
    void moveForward(float distance);
    void moveRight(float distance);
    void rotate(float deltaAlpha, float deltaFi);


};
