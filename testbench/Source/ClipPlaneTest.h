#pragma once

#include "TestSceneBase.h"
#include "../../j3d/Program.h"
#include "../../j3d/Camera.h"
#include "../../j3d/Mesh.h"
#include "../../j3d/SceneObject.h"
#include "../../j3d/environment.h"
#include "../../j3d/Geometries/GeometrySphere.h"

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
	GLuint pickingBuffer;
	GLuint pickingDepthBuffer;
	GLuint pickingTexture;

	void initPickingBuffer(int resX,int resY)
	{
		glGenFramebuffers(1, &pickingBuffer);
		glBindFramebuffer(GL_FRAMEBUFFER,pickingBuffer);

		glGenTextures(1, &pickingTexture);
		glBindTexture(GL_TEXTURE_2D, pickingTexture);
		glTexImage2D(GL_TEXTURE_2D,0, GL_RGB,resX, resY, 0, GL_RGB, GL_UNSIGNED_BYTE, (void*)0);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glBindTexture(GL_TEXTURE_2D, 0);

		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, pickingTexture, 0);
		GLenum *drawBuffers = new GLenum[1];
		drawBuffers[0] = { GL_COLOR_ATTACHMENT0 };
		glDrawBuffers(1, drawBuffers);

		glReadBuffer(GL_NONE);

		glGenRenderbuffers(1, &pickingDepthBuffer);
		glBindRenderbuffer(GL_RENDERBUFFER, pickingDepthBuffer);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, resX,resY);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, pickingDepthBuffer);

		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
			DBG("Framebuffer creation error: " + std::to_string(glCheckFramebufferStatus(GL_FRAMEBUFFER)) + " : " + std::to_string(glGetError()));
		else
			DBG("Framebuffer created!");

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	GLuint radMapSky = 0;
	GLuint irradMapSky = 0;
	GLuint bdrf_lut = 0;
	Texture* ourMatCap = 0;
	program* theProgram = 0;
	camera* theCamera = 0;
	glm::vec3 lightColor = { 1,1,1 };
	glm::vec3 lightPosition = { 10,7,5 };
	meshInstance scene;

	CameraOrbitControls* orbit_controls;

	juce::Draggable3DOrientation orientation;
};