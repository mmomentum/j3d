#include "Model.h"

namespace j3d
{
	Model::Model(GLuint program)
	{
		clipPlaneDirection = glm::normalize(clipPlaneDirection);

		glGenVertexArrays(1, &vao);
		glGenBuffers(1, &vertexBuffer);
		glGenBuffers(1, &normalBuffer);

		uniform_clipPlaneDirection = glGetUniformLocation(program, "clipPlaneDirection");
		uniform_clipPlaneOffset = glGetUniformLocation(program, "clipPlaneOffset");
	}

	Model::~Model()
	{
		glDeleteVertexArrays(1, &vao);
		glDeleteBuffers(1, &vertexBuffer);
		glDeleteBuffers(1, &normalBuffer);
	}

	/*
	void Model::createFromAcousticTriangles(std::vector<AcousticTriangle>& scene)
	{
		glBindVertexArray(vao);

		std::vector<glm::vec3> verts;
		for (int a = 0; a < scene.size(); a++)
		{
			verts.push_back(scene[a].v[0]);
			verts.push_back(scene[a].v[1]);
			verts.push_back(scene[a].v[2]);
		}
		numVerts = verts.size();

		//Can be changed to use instancing unless you want to actually import a model with per vertex normals
		std::vector<glm::vec3> normals;
		for (int a = 0; a < scene.size(); a++)
		{
			normals.push_back(scene[a].normal);
			normals.push_back(scene[a].normal);
			normals.push_back(scene[a].normal);
		}

		glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
		glBufferData(GL_ARRAY_BUFFER, verts.size() * sizeof(glm::vec3), &verts[0][0], GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

		glBindBuffer(GL_ARRAY_BUFFER, normalBuffer);
		glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(glm::vec3), &normals[0][0], GL_STATIC_DRAW);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

		glBindVertexArray(0);
	}
	*/

	void Model::render()
	{
		if (doClipping)
		{
			glEnable(GL_CLIP_DISTANCE0);
			glUniform3f(uniform_clipPlaneDirection, clipPlaneDirection.x, clipPlaneDirection.y, clipPlaneDirection.z);
			glUniform1f(uniform_clipPlaneOffset, clipOffset);
		}

		glBindVertexArray(vao);
		glDrawArrays(GL_TRIANGLES, 0, numVerts);
		glBindVertexArray(0);

		if (doClipping)
			glDisable(GL_CLIP_DISTANCE0);
	}
}
