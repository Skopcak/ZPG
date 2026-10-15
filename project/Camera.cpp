#include "Camera.h"
#include "ShaderProgram.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/geometric.hpp>

Camera::Camera(ShaderProgram* shaderProgram)
{
	m_shaderProgram = shaderProgram;

	viewMatrix = glm::mat4(1.0f);

	eye = glm::vec3(0.0f, 0.0f, 3.0f);
	target = glm::vec3(0.0f, 0.0f, -1.0f);
	up = glm::vec3(0.0f, 1.0f, 0.0f);

	updateProjection(800, 600);
}

glm::mat4 Camera::getCamera() {
	viewMatrix = glm::lookAt(eye, eye + target, up);
	return viewMatrix;
}

glm::mat4 Camera::getProjectionMatrix() const {
	return projectionMatrix;
}

void Camera::updateProjection(int width, int height)
{
	if (width <= 0 || height <= 0)
	{
		return;
	}
	projectionMatrix = glm::perspective(
		glm::radians(60.0f),
		static_cast<float>(width) / static_cast<float>(height),
		0.1f,
		100.0f
	);
}

void Camera::moveForward(float distance){
	glm::vec3 forward(target.x, 0.0f, target.z);
	eye += glm::normalize(forward) * distance;
}

void Camera::moveRight(float distance)
{
	glm::vec3 right = glm::normalize(glm::cross(target, up));
	eye += right * distance;
}
