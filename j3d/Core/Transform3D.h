#pragma once

#include "../Common.h"

class Transform3D
{
public:
	glm::vec3 position;
	glm::quat rotation;
	glm::vec3 scale;

	Transform3D()
		: position(glm::vec3(0.0f)),
		rotation(glm::quat(1.0f, 0.0f, 0.0f, 0.0f)),
		scale(glm::vec3(1.0f))
	{}

	void addPosition(const glm::vec3& delta) 
	{
		position += delta;
	}

	void addRotation(float angle, const glm::vec3& axis) 
	{
		glm::quat deltaRotation = glm::angleAxis(glm::radians(angle), glm::normalize(axis));
		rotation = deltaRotation * rotation;
	}

	void addScale(const glm::vec3& delta) 
	{
		scale += delta;
	}

	// Add to the position on a per-axis basis
	void addPosition(float deltaX, float deltaY, float deltaZ)
	{
		addPosition(glm::vec3(deltaX, deltaY, deltaZ));
	}

	// add to the rotation on a per-axis basis (angles in degrees)
	void addRotationX(float angle) 
	{
		addRotation(angle, glm::vec3(1.0f, 0.0f, 0.0f));
	}

	void addRotationY(float angle)
	{
		addRotation(angle, glm::vec3(0.0f, 1.0f, 0.0f));
	}

	void addRotationZ(float angle)
	{
		addRotation(angle, glm::vec3(0.0f, 0.0f, 1.0f));
	}

	// Add to the scale on a per-axis basis
	void addScale(float deltaX, float deltaY, float deltaZ) 
	{
		addScale(glm::vec3(deltaX, deltaY, deltaZ));
	}

	// Point the transform at another position
	void lookAt(const glm::vec3& targetPosition, const glm::vec3& up = glm::vec3(0.0f, 1.0f, 0.0f)) 
	{
		glm::mat4 lookAtMatrix = glm::lookAt(position, targetPosition, up);
		rotation = glm::quat_cast(glm::inverse(lookAtMatrix));
	}

	// Point the transform at another transform
	void lookAt(const Transform3D& targetTransform, const glm::vec3& up = glm::vec3(0.0f, 1.0f, 0.0f)) 
	{
		lookAt(targetTransform.position, up);
	}
};