#pragma once

#include "TestSceneBase.h"
#include "../../j3d/Scene.h"

using namespace juce::gl;

class ClipPlaneTest : public TestSceneBase
{
public:
	ClipPlaneTest();
	~ClipPlaneTest();

	// Inherited via TestSceneBase
	virtual void initialise() override;
	virtual void shutdown() override;
	virtual void render() override;

	void mouseDown(const juce::MouseEvent& e) override
	{
		orientation.mouseDown(e.getPosition());
	}

	void mouseDrag(const juce::MouseEvent& e) override
	{
		orientation.mouseDrag(e.getPosition());
	}

	void mouseWheelMove(const juce::MouseEvent&, const juce::MouseWheelDetails& w)
	{
		//zoom_value += (w.deltaY / 10.0f);
		//zoom_value = juce::jlimit(0.001f, 10.0f, zoom_value);
	}

	void resized() override;

private:
	//todo: abstract all of this away (uniforms into their own class, camera stuff into a 
	// "camera" class, etc)
	scene* testScene = 0;

	bool recentlyRecreatedRoom = false;

	juce::Draggable3DOrientation orientation;
};