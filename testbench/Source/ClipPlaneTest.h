#pragma once

#include "TestSceneBase.h"
#include "../../j3d/Program.h"
#include "../../j3d/Camera.h"
#include "../../j3d/Mesh.h"
#include "../../j3d/SceneObject.h"

using namespace juce::gl;

class ClipPlaneTest : public TestSceneBase, juce::KeyListener
{
public:
	ClipPlaneTest();
	~ClipPlaneTest();

	// Inherited via TestSceneBase
	virtual void initialise() override;
	virtual void shutdown() override;
	virtual void render() override;

	bool keyStateChanged(bool isKeyDown, Component* asdf) override
	{
		if (!cameraForwardKeyDown  && juce::KeyPress::isKeyCurrentlyDown(juce::KeyPress::numberPad8))
		{
			cameraForwardKeyDown = true;
			return true;
		}

		if (cameraForwardKeyDown && !juce::KeyPress::isKeyCurrentlyDown(juce::KeyPress::numberPad8))
		{
			cameraForwardKeyDown = false;
			return true;
		}

		return false;
	}

	void mouseUp(const juce::MouseEvent& e) override
	{
		theCamera->endTurn();
	}

	void mouseDrag(const juce::MouseEvent& e) override
	{
		theCamera->turn(e.getDistanceFromDragStartX() * 0.003, e.getDistanceFromDragStartY() * 0.003);
	}

	void mouseWheelMove(const juce::MouseEvent&, const juce::MouseWheelDetails& w)
	{
		//zoom_value += (w.deltaY / 10.0f);
		//zoom_value = juce::jlimit(0.001f, 10.0f, zoom_value);
	}

	void resized() override;

private:

	bool cameraForwardKeyDown = false;

	program* theProgram = 0;
	camera* theCamera = 0;
	glm::vec3 lightColor = { 1,1,1 };
	glm::vec3 lightPosition = { 10,7,5 };
	std::vector<meshObject*> scene;

	juce::Draggable3DOrientation orientation;
};