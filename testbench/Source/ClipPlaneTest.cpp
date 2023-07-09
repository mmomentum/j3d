#include "ClipPlaneTest.h"

ClipPlaneTest::ClipPlaneTest()
{
	addTestParameter("clip", 0.0f, 1.0f, 0.5f);
	addTestParameter("anotherparam", 0.0f, 5.0f, 4.0f);
}

ClipPlaneTest::~ClipPlaneTest()
{

}

void ClipPlaneTest::initialise()
{
	//Use default/sample shaders
	mainShader = new program();

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glEnable(GL_DEPTH_TEST);

	/*glDisable(GL_CULL_FACE);
	glCullFace(GL_BACK);*/

	uniform_cameraProjection = mainShader->getUniformLocation("cameraProjection");
	uniform_cameraView = mainShader->getUniformLocation("cameraView");
	uniform_cameraScale = mainShader->getUniformLocation("scale");
	uniform_isRay = mainShader->getUniformLocation("isRay");

	matrix_cameraProjection = glm::ortho(-2.0, 2.0, -2.0, 2.0, -2.0, 10.0);

	//Creates a default/sample cube mesh:
	testMesh = new Mesh();
}
 
void ClipPlaneTest::shutdown()
{
	shutdownOpenGL();
}

void ClipPlaneTest::render()
{
	glDisable(GL_CULL_FACE);
	glDisable(GL_BLEND);
	glDisable(GL_CLIP_DISTANCE0);
	glEnable(GL_DEPTH_TEST);

	glClearColor(1, 0.5, 0.2, 1);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	mainShader->use();

	glUniformMatrix4fv(uniform_cameraProjection, 1, GL_FALSE, &matrix_cameraProjection[0][0]);
	memcpy(&matrix_cameraView, orientation.getRotationMatrix().mat, sizeof(float) * 16);
	glUniformMatrix4fv(uniform_cameraView, 1, GL_FALSE, &matrix_cameraView[0][0]);
	glUniform1f(uniform_cameraScale, zoom_value);

	testMesh->render();

	/*if (recentlyRecreatedRoom)
	{
		if (room)
		{
			delete room;
			room = 0;
		}

		room = new j3d::Model(program);
		// room->createFromAcousticTriangles(triangles);

		recentlyRecreatedRoom = false;
	}

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	mainShader->use();

	glUniformMatrix4fv(uniform_cameraProjection, 1, GL_FALSE, &matrix_cameraProjection[0][0]);
	memcpy(&matrix_cameraView, orientation.getRotationMatrix().mat, sizeof(float) * 16);
	//matrix_cameraView = glm::lookAt(glm::vec3(-2, -2, -2), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
	glUniformMatrix4fv(uniform_cameraView, 1, GL_FALSE, &matrix_cameraView[0][0]);
	glUniform1f(uniform_cameraScale, zoom_value); 

	//glEnable(GL_BLEND);
	glDisable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_DEPTH_TEST);

	if (room)
	{
		room->clipOffset = getParameterValue("clip");
		room->render();
	}*/
}

void ClipPlaneTest::resized()
{
	TestSceneBase::resized();

	orientation.setViewport(getLocalBounds());
}