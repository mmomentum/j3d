#pragma once

#include <JuceHeader.h>

#include <glm.hpp>

// 3d model
namespace j3d
{
	using namespace juce::gl;

	struct Model
	{
		GLuint uniform_clipPlaneDirection;
		GLuint uniform_clipPlaneOffset;

		glm::vec3 clipPlaneDirection = glm::vec3(1.0, 1.0, 0);
		float clipOffset = 0.5;
		bool doClipping = true;

		GLuint vao;
		GLuint vertexBuffer;
		GLuint normalBuffer;
		int numVerts = 0;

		// make this do something else
		// void createFromAcousticTriangles(std::vector<AcousticTriangle>& scene);
		void render();

		Model(GLuint program);
		~Model();
	};
}