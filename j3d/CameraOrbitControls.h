#pragma once

#include <JuceHeader.h>

#include "Camera.h"

#include "glm/gtc/matrix_transform.hpp"
#include "glm/mat4x4.hpp"

class CameraOrbitControls : private juce::MouseListener, juce::KeyListener
{
public:
	CameraOrbitControls(camera& Camera, juce::Component& component);

private:
	camera& camera_;
	juce::Component& component_scene_;

	float zoom_distance_ = 4.0f;

	float azimuth = 45.0f, elevation = 10.0f;

	glm::vec3 origin_point = { 0.0f, 0.0f, 0.0f };

	juce::Point<int> lastMousePosition;
	glm::vec3 lastCameraPosition;

	// takes the three values and sets the cameras direction + position accordingly
	void updatePosition();

	void mouseDown(const juce::MouseEvent& event) override;
	void mouseDrag(const juce::MouseEvent& event) override;

	void mouseWheelMove(const juce::MouseEvent& event, const juce::MouseWheelDetails& wheel);

	bool keyPressed(const juce::KeyPress& key, juce::Component* originatingComponent) override;
};