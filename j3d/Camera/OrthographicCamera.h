#pragma once

#include "CameraBase.h"

class OrthographicCamera : public Camera
{
public:
	float ortho_scale = 1.0f;

	const void render(juce::OpenGLShaderProgram* currentProgram);

	glm::mat4 getProjectionMatrix() override
	{
		float ortho_tmp = ortho_scale * zoom_value;

		// Assuming left, right, bottom, and top define the boundaries of the orthographic projection
		// You might need to adjust these values based on your specific requirements
		float left = -1.0f * ortho_tmp * aspectRatio;   // Adjust as needed
		float right = 1.0f * ortho_tmp * aspectRatio;   // Adjust as needed
		float bottom = -1.0f * ortho_tmp; // Adjust as needed
		float top = 1.0f * ortho_tmp;     // Adjust as needed

		return glm::ortho(left, right, bottom, top, -farPlane, farPlane);
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