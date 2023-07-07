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
	auto vertexSource = R"V0G0N(
	#version 400 core

	layout(location = 0) in vec3 vertexPosition;
	layout(location = 1) in vec3 vertexNormal;

	uniform mat4 cameraView;
	uniform mat4 cameraProjection;
	uniform float scale;
	uniform bool isRay;
	uniform vec3 clipPlaneDirection;
	uniform float clipPlaneOffset;

	out vec3 normal;

	void main()
	{
		normal = vertexNormal;
		gl_Position = cameraProjection * cameraView * vec4(scale * vertexPosition,1.0);

		gl_ClipDistance[0] = clipPlaneOffset + clipPlaneDirection.x * vertexPosition.x +  clipPlaneDirection.y * vertexPosition.y +  clipPlaneDirection.z * vertexPosition.z;
	}
	)V0G0N";

	auto fragmentSource = R"V0G0N(
	#version 400 core

	uniform bool isRay;

	in vec3 normal;

	out vec4 color;

	void main()
	{
		if(isRay)
			color = vec4(1.0,0.0,0.0,1.0);	
		else
			color = vec4(0.0,0.0,1.0,1.0);
	}
	)V0G0N";

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glEnable(GL_DEPTH_TEST);

	glDisable(GL_CULL_FACE);
	glCullFace(GL_BACK);

	GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
	GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);

	std::string vertexString = std::string(vertexSource);
	std::string fragmentString = std::string(fragmentSource);

	GLint fragmentLength = sizeof(GLchar) * fragmentString.length();
	GLint vertexLength = sizeof(GLchar) * vertexString.length();

	glShaderSource(vertexShader, 1, &vertexSource, &vertexLength);
	glShaderSource(fragmentShader, 1, &fragmentSource, &fragmentLength);

	glCompileShader(vertexShader);
	glCompileShader(fragmentShader);

	int length = 0, actualLength = 0;

	{
		glGetShaderiv(vertexShader, GL_INFO_LOG_LENGTH, &length);
		char* data = new char[length];
		glGetShaderInfoLog(vertexShader, length, &actualLength, data);

		if (length != actualLength)
			DBG("Vertex shader log not expected length!");
		if (length == 0 || actualLength == 0)
			DBG("No vertex shader log returned!");
		else
			DBG(std::string(data));

		delete data;
	}

	{
		glGetShaderiv(fragmentShader, GL_INFO_LOG_LENGTH, &length);
		char* data = new char[length];
		glGetShaderInfoLog(fragmentShader, length, &actualLength, data);

		if (length != actualLength)
			DBG("Fragment shader log not expected length!");
		if (length == 0 || actualLength == 0)
			DBG("No fragment shader log returned!");
		else
			DBG(std::string(data));

		delete data;
	}

	program = glCreateProgram();

	glAttachShader(program, vertexShader);
	glAttachShader(program, fragmentShader);

	glLinkProgram(program);
	GLint programSuccess = GL_FALSE;
	glGetProgramiv(program, GL_LINK_STATUS, &programSuccess);

	if (programSuccess != GL_TRUE)
	{
		DBG("Could not create program!");

		glGetProgramiv(program, GL_INFO_LOG_LENGTH, &length);
		char* data = new char[length];
		glGetProgramInfoLog(program, length, &actualLength, data);

		if (length != actualLength)
			DBG("Program log not expected length!");
		if (length == 0 || actualLength == 0)
			DBG("No program log returned!");
		else
			DBG(std::string(data));

		delete data;
	}

	DBG("Program link succesfull!\n\n");

	uniform_cameraProjection = glGetUniformLocation(program, "cameraProjection");
	uniform_cameraView = glGetUniformLocation(program, "cameraView");
	uniform_cameraScale = glGetUniformLocation(program, "scale");
	uniform_isRay = glGetUniformLocation(program, "isRay");

	matrix_cameraProjection = glm::ortho(-2.0, 2.0, -2.0, 2.0, -2.0, 10.0);
}

void ClipPlaneTest::shutdown()
{
}

void ClipPlaneTest::render()
{
	if (recentlyRecreatedRoom)
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

	glUseProgram(program);

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
	}
}

void ClipPlaneTest::resized()
{
	TestSceneBase::resized();

	orientation.setViewport(getLocalBounds());
}