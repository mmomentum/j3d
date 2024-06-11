#pragma once

#include "CameraControlsBase.h"

class CameraOrbitControls : public CameraControlsBase
{
public:
	CameraOrbitControls(Camera*, juce::Component& component);

	~CameraOrbitControls();

	void setNewCamera(Camera*) override;

	virtual juce::String serializeControlState() override;
	virtual void deserializeControlState(juce::String) override;

	// add azimuth & elevation functions for interfacing with orbit controls via the numpad (blenderlike)
	void addAzimuth(float azimuth);
	void addElevation(float elevation);

private:
	// takes the three values and sets the cameras direction + position accordingly
	void updatePosition();

	void mouseDown(const juce::MouseEvent& event) override;
	void mouseDrag(const juce::MouseEvent& event) override;

	void mouseWheelMove(const juce::MouseEvent& event, const juce::MouseWheelDetails& wheel);

	bool keyPressed(const juce::KeyPress& key, juce::Component* originatingComponent) override;

	void initializeValues();

	float azimuth = 0.0f, elevation = 0.0f;

	glm::vec3 origin_point = { 0.0f, 0.0f, 0.0f };

	juce::Point<int> lastMousePosition;
	glm::vec3 lastCameraPosition;
};