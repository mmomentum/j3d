#pragma once

#include "CameraBase.h"

class PerspectiveCamera : public Camera
{
public:
	float fov = 90.0f; // value is in degrees

	const void render(juce::OpenGLShaderProgram* currentProgram);

	glm::mat4 getProjectionMatrix() override
	{
		return  glm::perspective(glm::radians(fov), aspectRatio, nearPlane, farPlane);
	}

	// Inherited via Camera
	virtual glm::vec3 unprojectMouseCoordinate(juce::Rectangle<int> screen_size, juce::Point<int> mouse_pos,
		glm::vec3 plane_origin) override;

	virtual glm::vec3 unprojectMouseCoordinate(juce::Rectangle<int> screen_size, juce::Point<int> mouse_pos,
		glm::vec3 plane_origin, glm::vec3 custom_plane_normal) override;

	virtual juce::Point<float> projectPositionCoordinate(juce::Rectangle<int> screen_size,
		glm::vec3 position) override;

	virtual glm::vec3 projectPositionCoordinateWithZ(juce::Rectangle<int> screen_size,
		glm::vec3 position) override;
};