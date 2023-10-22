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
	initPickingBuffer(getWidth(), getHeight());
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

	/*Texture incoming_texture(BinaryData::output_pmrem_png, BinaryData::output_pmrem_pngSize);
	radMapSky = processEquirectangularMap(*rectToCube, incoming_texture, true);

	Texture incoming_texture2(BinaryData::output_iem_png, BinaryData::output_iem_pngSize);
	irradMapSky = processEquirectangularMap(*rectToCube, incoming_texture2, true);*/

	radMapSky = processEquirectangularMap(*rectToCube, "output_pmrem.hdr", true);
	irradMapSky = processEquirectangularMap(*rectToCube, "output_iem.hdr", true);

	bdrf_lut = createBDRFLUTTexture();
	 
	GLenum err = glGetError();
	if (err != GL_NO_ERROR)
	{
		std::cout << "OpenGL error " << err << "\n\n";
	}
	 
	//Creates a default/sample cube mesh:

	/*GeometrySphere::Params test;
	test.widthSegments = 16*3;
	test.heightSegments = 12*3;
	test.radius = 1.0;
	test.phiStart = 0,
	test.phiLength = juce::MathConstants<float>::twoPi,
	test.thetaStart = 0,
	test.thetaLength = juce::MathConstants<float>::pi;
	Mesh* testMesh = new Mesh(GeometrySphere::create(test));*/
	Mesh *testMesh = new Mesh(BinaryData::head_obj, BinaryData::head_objSize);

	//testTexture = new Texture({ 100,100 });
	Texture* brickAlbedo = new Texture(BinaryData::brick_albedo_png, BinaryData::brick_albedo_pngSize);
	//Texture *testTexture = new Texture("G:/2003-2008 hd one/cshot2noa.png");
	Material* testMaterial = new Material;
	testMaterial->setTexture(brickAlbedo, albedo);
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

	/*
	meshInstance* ghost = new meshInstance;
	ghost->scale = glm::vec3(0.3, 0.3, 0.3);
	ghost->theMaterial = testMaterial;
	ghost->add(testMesh,glm::vec3(0,5,0))->add(testMesh, glm::vec3(5, 0, 0))->add(testMesh,glm::vec3(5,0,0))->add(testMesh, glm::vec3(5, 0, 0));
	ghost->add(testMesh, glm::vec3(0, 0, 5), glm::quat(glm::vec3(glm::radians(45.0),0,0)));
	ghost->add(testMesh)->add(testMesh, glm::vec3(0, 0, 0),glm::quat(1,0,0,0),secondMaterial);
	scene.push_back(ghost);
	*/

	for (int a = 0; a < 16; a++)
	{
		for (int b = 0; b < 16; b++)
		{
			scene.add(testMesh,glm::vec3(a * 2, b * 2, 0));
		}
	}

	setWantsKeyboardFocus(true);
}
 
void ClipPlaneTest::shutdown()
{
	//shutdownOpenGL();
}

int lastPicked = 0;

void ClipPlaneTest::render()
{
	glBindFramebuffer(GL_FRAMEBUFFER, pickingBuffer);
	glViewport(0, 0, getWidth(),getHeight());
	glClearColor(0,0,0,0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	theProgram->use();
	((perspectiveCamera*)theCamera)->render(theProgram);

	glUniform1i(theProgram->getUniformLocation("usePickingColor"), true);
	scene.renderPicking(theProgram);
	glUniform1i(theProgram->getUniformLocation("usePickingColor"), false);

	glFlush();
	glFinish();

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glBindFramebuffer(GL_READ_FRAMEBUFFER, pickingBuffer);

	glReadBuffer(GL_COLOR_ATTACHMENT0);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	int mx = 50, my = 50;

	unsigned char data[3] = { 0,0,0 };
	glReadPixels(mx, my, 1, 1, GL_RGB, GL_UNSIGNED_BYTE, data);

	int pickedID = data[2] + data[1] << 8 + data[0] << 16;
	if(lastPicked != pickedID)
		DBG("Picked: " + std::to_string((unsigned int)data[0]) + " " + std::to_string((unsigned int)data[1]) + " " + std::to_string((unsigned int)data[2]));

	lastPicked = pickedID;

	glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);

	//glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glEnable(GL_DEPTH_TEST);
	glDisable(GL_BLEND);

	glClearColor(1, 0.5, 0.2, 1);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glActiveTexture(GL_TEXTURE0 + radMap);
	glBindTexture(GL_TEXTURE_CUBE_MAP, radMapSky);
	glActiveTexture(GL_TEXTURE0 + irradMap);
	glBindTexture(GL_TEXTURE_CUBE_MAP, irradMapSky);

	glActiveTexture(GL_TEXTURE0 + bdrf);
	glBindTexture(GL_TEXTURE_2D, bdrf_lut);

	theProgram->use();
	((perspectiveCamera*)theCamera)->render(theProgram);

	theProgram->setLightColor(lightColor);
	theProgram->setLightPosition(lightPosition);

	scene.render(theProgram);

	GLenum err = glGetError();
	if (err != GL_NO_ERROR)
	{
		std::cout << "OpenGL error " << err << "\n\n";
	}
}

void ClipPlaneTest::resized()
{
	TestSceneBase::resized();

	orientation.setViewport(getLocalBounds());
}