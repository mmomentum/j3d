#pragma once

#include <JuceHeader.h>

#include "Camera.h"

#include "glm/gtc/matrix_transform.hpp"
#include "glm/mat4x4.hpp"

// mousewheel zooms + arrow keys orbit. this is the best youll get for now
class CameraOrbitControls : private juce::MouseListener, juce::KeyListener
{
public:
	CameraOrbitControls(camera& camera, juce::Component& component);

private:
	camera& camera_;
	juce::Component& component_scene_;

	float zoom_distance_ = 2.0f;

	float azimuth = 45.0f, elevation = 45.0f;

	// takes the three values and sets the cameras direction + position accordingly
	void updatePosition();

	void mouseDrag(const juce::MouseEvent & event);

	void mouseWheelMove(const juce::MouseEvent& event, const juce::MouseWheelDetails& wheel);

	bool keyPressed(const juce::KeyPress& key, juce::Component* originatingComponent) override;
};