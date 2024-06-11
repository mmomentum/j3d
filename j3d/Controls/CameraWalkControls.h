#pragma once

#include "CameraControlsBase.h"

class CameraWalkControls : public CameraControlsBase,
	private juce::Timer
{
public:
	CameraWalkControls(Camera*, juce::Component&);
	~CameraWalkControls();

	void setNewCamera(Camera*) override;

	void setViewAngle(juce::CommandID id) override;

	// Inherited via CameraControlsBase
	juce::String serializeControlState() override;
	void deserializeControlState(juce::String) override;

	void moveUp(float delta_time);
	void moveDown(float delta_time);

	void moveForwardsBackwards(const juce::MouseWheelDetails&);

	void moveLeft(float delta_time);
	void moveRight(float delta_time);

private:
	void mouseDown(const juce::MouseEvent&) override;
	void mouseDrag(const juce::MouseEvent&) override;

	void mouseWheelMove(const juce::MouseEvent& event, const juce::MouseWheelDetails& wheel);

	// calls at a consistent rate as long as this control scheme exists. handles movement internally
	// based on which move keys are being pressed down
	void timerCallback() override;

	void updateDirection(float delta_x, float delta_y);

	glm::vec3 getRelativeUp();

	int last_mouse_x = 0, last_mouse_y = 0;

	float move_speed = 2.0f;

	float azimuth = 0.0f, elevation = 0.0f;

	juce::Time lastUpdateTime;

	juce::Array<juce::KeyPress> key_presses;
	int keypress_indexes[2][4];
};