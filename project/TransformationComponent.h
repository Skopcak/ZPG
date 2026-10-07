#pragma once

#include <glm/mat4x4.hpp>

class TransformationComponent {
	public:
		virtual ~TransformationComponent() = default;
		virtual glm::mat4 getMatrix() const = 0;
};
