#include "Camera.h"
#include "CameraObserver.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/geometric.hpp>
#include <algorithm>
#include <cmath>

Camera::Camera(){
	eye = glm::vec3(0.0f, 0.0f, 3.0f);
	target = glm::vec3(0.0f, 0.0f, -1.0f);
	up = glm::vec3(0.0f, 1.0f, 0.0f);

	alpha = glm::radians(90.0f);
	fi = glm::radians(-90.0f);

	updateView();
	updateProjection(800, 600);
}

void Camera::updateView(){
	viewMatrix = glm::lookAt(eye, eye + target, up);
	notifyObservers();
}

glm::mat4 Camera::getCamera() const{
	return viewMatrix;
}

glm::mat4 Camera::getProjectionMatrix() const {
	return projectionMatrix;
}

void Camera::updateProjection(int width, int height){
	if (width <= 0 || height <= 0){
		return;
	}
	if (width == projectionWidth && height == projectionHeight) {
		return;
	}

	projectionMatrix = glm::perspective(
		glm::radians(90.0f),
		static_cast<float>(width) / static_cast<float>(height),
		0.1f,
		100.0f
	);
	projectionWidth = width;
	projectionHeight = height;
	notifyObservers();
}

void Camera::moveForward(float distance){
	if (distance == 0.0f){
		return;
	}

	// Fly along the full viewing direction, including the camera's pitch.
	eye += glm::normalize(target) * distance;
	updateView();
}

void Camera::moveRight(float distance){
	if (distance == 0.0f){
		return;
	}

	glm::vec3 right = glm::normalize(glm::cross(target, up));
	eye += right * distance;
	updateView();
}

void Camera::rotate(float deltaAlpha, float deltaFi) {
	if (deltaAlpha == 0.0f && deltaFi == 0.0f){
		return;
	}

	alpha += deltaAlpha;
	fi += deltaFi;

	alpha = std::clamp(
		alpha,
		glm::radians(1.0f),
		glm::radians(179.0f)

	);

	fi = std::remainder(fi, glm::radians(360.0f));

	target.x = std::sin(alpha) * std::cos(fi);
	target.z = std::sin(alpha) * std::sin(fi);
	target.y = std::cos(alpha);
	updateView();
}


glm::vec3 Camera::getPosition() const{
    return eye;
}

void Camera::addObserver(CameraObserver& observer){
	if (std::find(observers.begin(), observers.end(), &observer)!= observers.end()){
		return;
	}

	observers.push_back(&observer);
	observer.onCameraChanged(*this);
}

void Camera::removeObserver(CameraObserver& observer){
	std::erase(observers, &observer);
}

void Camera::notifyObservers(){
	for (CameraObserver* observer : observers){
		observer->onCameraChanged(*this);
	}
}
