#include "ClipPlaneTest.h"

ClipPlaneTest::ClipPlaneTest()
{
	addTestParameter("clip", -0.8f, 0.8f, 0.0f);
	addTestParameter("anotherparam", 0.0f, 5.0f, 4.0f);
}

ClipPlaneTest::~ClipPlaneTest()
{
	shutdownOpenGL();
}

void ClipPlaneTest::initialise()
{
	//Use default/sample shaders

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glEnable(GL_DEPTH_TEST);

	/*glDisable(GL_CULL_FACE);
	glCullFace(GL_BACK);*/

	/*uniform_cameraProjection = mainShader->getUniformLocation("cameraProjection");
	uniform_cameraView = mainShader->getUniformLocation("cameraView");
	uniform_cameraScale = mainShader->getUniformLocation("scale");
	uniform_isRay = mainShader->getUniformLocation("isRay");

	matrix_cameraProjection = glm::ortho(-2.0, 2.0, -2.0, 2.0, -2.0, 10.0);*/

	testScene = new scene;
	testScene->toUse = new program();

	testScene->camera = new orthoCamera;
	((orthoCamera*)testScene->camera)->near = glm::vec3(-2.0, -2.0, -2.0);
	((orthoCamera*)testScene->camera)->far = glm::vec3(2.0, 2.0, 2.0);
	((orthoCamera*)testScene->camera)->position = glm::vec3(0,0,-10);
	((orthoCamera*)testScene->camera)->rotation = glm::quat(1.0, 0.0, 0.0, 0.0);

	meshObject* cube = new meshObject; 
	 
	//Creates a default/sample cube mesh:
	Mesh *testMesh = new Mesh();

	//testTexture = new Texture({ 100,100 });
	Texture *testTexture = new Texture("G:\\2003-2008 hd one\\cshot2noa.png");
	Material* testMaterial = new Material;
	testMaterial->setTexture(testTexture, albedo);
	testMaterial->enableClipping(glm::vec3(1.0,1.0,0.0), 0);

	cube->toUse = testMaterial;
	cube->meshes.push_back(testMesh);
	cube->meshPositions.push_back({0,0,0});
	cube->meshRotations.push_back({1,0,0,0});

	testScene->scene.push_back(cube);
}
 
void ClipPlaneTest::shutdown()
{
	//shutdownOpenGL();
}

void ClipPlaneTest::render()
{
	//if (scene.size() > 0)
		//scene[0]->toUse->enableClipping(glm::vec3(1.0, 1.0, 0.0), getTestParameter("clip").getValue());

	//glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glEnable(GL_DEPTH_TEST);
	glDisable(GL_BLEND);

	glClearColor(1, 0.5, 0.2, 1);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	testScene->render(); 

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