#pragma once

#include "TestSceneBase.h"

#include "../../j3d/Model.h"

using namespace juce::gl;

#include <../glm/gtx/transform.hpp>

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
		zoom_value += (w.deltaY / 10.0f);
		zoom_value = juce::jlimit(0.001f, 10.0f, zoom_value);
	}

private:
	//todo: abstract all of this away (uniforms into their own class, camera stuff into a 
	// "camera" class, etc)
	GLuint program = 0;

	GLuint uniform_cameraView;
	GLuint uniform_cameraScale;
	GLuint uniform_cameraProjection;
	GLuint uniform_isRay;
	glm::mat4 matrix_cameraProjection = glm::mat4(1.0);
	glm::mat4 matrix_cameraView = glm::mat4(1.0);

	bool recentlyRecreatedRoom = false;

	j3d::Model* room = 0;

	juce::Draggable3DOrientation orientation;

	float zoom_value = 0.5f;
};