#pragma once

#include "../Common.h"

#include "../Core/Transform3D.h"

class Camera : public Transform3D
{
public:
	static glm::quat directionToQuaternion(const glm::vec3& direction, const glm::vec3& up = glm::vec3(0.0f, 1.0f, 0.0f)) {
		glm::vec3 forward = glm::normalize(direction);
		glm::vec3 right = glm::normalize(glm::cross(up, forward));
		glm::vec3 realUp = glm::cross(forward, right);

		glm::mat3 rotationMatrix(right, realUp, forward);
		return glm::quat_cast(rotationMatrix);
	}

	static glm::vec3 quaternionToDirection(const glm::quat& quat) {
		glm::vec3 forward(0.0f, 0.0f, -1.0f);
		return glm::rotate(quat, forward);
	}

	float nearPlane = 0.05f;
	float farPlane = 100.0f;

	float aspectRatio = 1.0f;

	float zoom_value = 1.0f;

	glm::vec3 up = { 0.0f, 1.0f, 0.0f };

	void copyFrom(const Camera& other)
	{
		position = other.position;
		rotation = other.rotation;
		scale = other.scale;

		up = other.up;

		aspectRatio = other.aspectRatio;
		zoom_value = other.zoom_value;
	}

	virtual const void render(juce::OpenGLShaderProgram* currentProgram) = 0;

	virtual glm::mat4 getProjectionMatrix() = 0;
	glm::mat4 getViewMatrix() { return glm::lookAt(position, position + quaternionToDirection(rotation), up); }

	// unprojects a mouse coordinate by casting a ray from the camera (based on the mouse position & screen size)
	// and intersects against a plane that exists relative to the camera at a distance of `camera_distance`.
	virtual glm::vec3 unprojectMouseCoordinate(juce::Rectangle<int> screen_size, juce::Point<int> mouse_pos,
		glm::vec3 plane_origin)
	{
		juce::ignoreUnused(screen_size, mouse_pos, plane_origin);

		return glm::vec3(0.0f, 0.0f, 0.0f);
	}

	virtual glm::vec3 unprojectMouseCoordinate(juce::Rectangle<int> screen_size, juce::Point<int> mouse_pos,
		glm::vec3 plane_origin, glm::vec3 custom_plane_normal)
	{
		juce::ignoreUnused(screen_size, mouse_pos, plane_origin, custom_plane_normal);

		return glm::vec3(0.0f, 0.0f, 0.0f);
	}

	// basically does the opposite of what unprojectMouseCoordinate does; we input a point in three dimensional
	// space and we figure out where that point would be in screenspace (so what the vertex shader does, but for
	// other uses beyond just putting stuff on-screen)
	virtual juce::Point<float> projectPositionCoordinate(juce::Rectangle<int> screen_size, glm::vec3 position)
	{
		juce::ignoreUnused(screen_size, position);

		return juce::Point<float>(0.0f, 0.0f);
	}

	// does what the above system does but retains the Z value as well (so you can know if something is in front 
	// of, or behind the camera)
	virtual glm::vec3 projectPositionCoordinateWithZ(juce::Rectangle<int> screen_size, glm::vec3 position)
	{
		juce::ignoreUnused(screen_size, position);

		return glm::vec3(0.0f, 0.0f, 0.0f);
	}
};