#include "ClipPlaneTest.h"

#include "../../j3d/Geometries/GeometryCube.h"
#include "../../j3d/Geometries/GeometryCylinder.h"
#include "../../j3d/Geometries/GeometryCone.h"
#include "../../j3d/Geometries/GeometryIcosahedron.h"
#include "../../j3d/Geometries/GeometrySphere.h"
#include "../../j3d/Geometries/GeometryTorus.h"

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

	/*theCamera = new orthoCamera;
	((orthoCamera*)theCamera)->near = glm::vec3(-1.0, -1.0, -1.0);
	((orthoCamera*)theCamera)->far = glm::vec3(1.0, 1.0, 1.0);*/
	
	theCamera = new perspectiveCamera;

	orbit_controls = new CameraOrbitControls(*theCamera, *this);

	//Get sky box made:

	program* rectToCube = rectToCubeProgram();

	Texture incoming_texture(BinaryData::skybox_jpg, BinaryData::skybox_jpgSize);

	GLuint skyBoxTexture = processEquirectangularMap(*rectToCube, incoming_texture, true);

	GLenum err = glGetError();
	if (err != GL_NO_ERROR)
	{
		std::cout << "OpenGL error " << err << "\n\n";
	}
	 
	//Creates a default/sample cube mesh:

	Mesh* testMesh = new Mesh(BinaryData::head_obj, BinaryData::head_objSize);

	//testTexture = new Texture({ 100,100 });
	Texture* testTexture = new Texture(BinaryData::map_png, BinaryData::map_pngSize);
	//Texture *testTexture = new Texture("G:/2003-2008 hd one/cshot2noa.png");
	Material* testMaterial = new Material;
	testMaterial->setTexture(testTexture, albedo);
	//testMaterial->enableClipping(glm::normalize(glm::vec3(1, 1, 0)), 0);

	Material* secondMaterial = new Material;
	secondMaterial->setTexture(new Texture(glm::ivec2(200,200)), albedo);



	ourMatCap = new Texture(BinaryData::goldmatcap_jpg, BinaryData::goldmatcap_jpgSize);
	/*meshObject* cube = new meshObject;
	cube->theMaterial = testMaterial;
	cube->theMesh = testMesh;
	cube->position = { 0,0,0 };
	cube->rotation = { 1,0,0,0 };
	scene.push_back(cube);*/

	meshInstance* ghost = new meshInstance;
	ghost->scale = glm::vec3(0.3, 0.3, 0.3);
	ghost->theMaterial = testMaterial;
	ghost->add(testMesh)->add(testMesh, glm::vec3(5, 0, 0))->add(testMesh,glm::vec3(5,0,0))->add(testMesh, glm::vec3(5, 0, 0));
	ghost->add(testMesh, glm::vec3(0, 0, 5), glm::quat(glm::vec3(glm::radians(45.0),0,0)));
	ghost->add(testMesh)->add(testMesh, glm::vec3(0, 5, 0),glm::quat(1,0,0,0),secondMaterial);
	scene.push_back(ghost);

	setWantsKeyboardFocus(true);
}
 
void ClipPlaneTest::shutdown()
{
	//shutdownOpenGL();
}

void ClipPlaneTest::render()
{
	//glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glEnable(GL_DEPTH_TEST);
	glDisable(GL_BLEND);

	glClearColor(1, 0.5, 0.2, 1);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	theCamera->render(theProgram);
	theProgram->use();

	ourMatCap->bind(matCap);

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