#pragma once

#include "TestSceneBase.h"
#include "../../j3d/Program.h"
#include "../../j3d/Camera.h"
#include "../../j3d/Mesh.h"
#include "../../j3d/SceneObject.h"

#include "../../j3d/CameraOrbitControls.h"

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

	void resized() override;

private:

	program* theProgram = 0;
	camera* theCamera = 0;
	glm::vec3 lightColor = { 1,1,1 };
	glm::vec3 lightPosition = { 10,7,5 };
	std::vector<meshObject*> scene;

	CameraOrbitControls* orbit_controls;

	juce::Draggable3DOrientation orientation;
};