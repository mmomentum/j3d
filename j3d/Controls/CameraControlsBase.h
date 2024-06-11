#pragma once

#include "../Common.h"

#include "../Camera/CameraBase.h"

class CameraControlsBase : public juce::MouseListener,
	public juce::KeyListener
{
public:
	CameraControlsBase(Camera* camera, juce::Component& viewport_owner);
	~CameraControlsBase();

	// takes in different camera angle snapping command IDs and configures the camera 
	// controller state to match the desired command
	virtual void setViewAngle(juce::CommandID id) = 0;

	// sets the internal camera pointer to a different one, and sets the camera's internal members
	// to be consistent with what was there prior (this is for doing camera switching when changing
	// between orthographic and perspective)
	virtual void setNewCamera(Camera* camera) = 0;

	// methods for loading & saving the camera controller's state. once you have
	// the serialized string you should hold it for later somewhere else
	virtual juce::String serializeControlState() = 0;
	virtual void deserializeControlState(juce::String) = 0;

	bool keyPressed(const juce::KeyPress&, juce::Component*) override { return false; }

	Camera* camera;
	juce::Component& owner;
};