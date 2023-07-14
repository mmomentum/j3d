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

	theProgram = new program();

	theCamera = new orthoCamera;
	((orthoCamera*)theCamera)->near = glm::vec3(-1.0, -1.0, -1.0);
	((orthoCamera*)theCamera)->far = glm::vec3(1.0, 1.0, 1.0);
	((orthoCamera*)theCamera)->position = glm::vec3(0, 0, -1);
	((orthoCamera*)theCamera)->direction = glm::vec3(0, 0, 1);

	//Creates a default/sample cube mesh:
	Mesh* testMesh = new Mesh();

	//testTexture = new Texture({ 100,100 });
	Texture* testTexture = new Texture({100,100});// ("G:\\2003-2008 hd one\\cshot2noa.png");
	Material* testMaterial = new Material;
	testMaterial->setTexture(testTexture, albedo);
	testMaterial->disableClipping();

	meshObject* cube = new meshObject;
	cube->theMaterial = testMaterial;
	cube->theMesh = testMesh;
	cube->position = { 0,0,0 };
	cube->rotation = { 1,0,0,0 };
	scene.push_back(cube);

	setWantsKeyboardFocus(true);
}
 
void ClipPlaneTest::shutdown()
{
	//shutdownOpenGL();
}

void ClipPlaneTest::render()
{
	//if (scene.size() > 0)
		//scene[0]->toUse->enableClipping(glm::vec3(1.0, 1.0, 0.0), getTestParameter("clip").getValue());\

	if (cameraForwardKeyDown)
		theCamera->moveForward(0.01);

	//glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glEnable(GL_DEPTH_TEST);
	glDisable(GL_BLEND);

	glClearColor(1, 0.5, 0.2, 1);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	theCamera->render(theProgram);
	theProgram->use();
	theProgram->setLightColor(lightColor);
	theProgram->setLightPosition(lightPosition);
	for (int a = 0; a < scene.size(); a++)
		scene[a]->render(theProgram);
}

void ClipPlaneTest::resized()
{
	TestSceneBase::resized();

	orientation.setViewport(getLocalBounds());
}