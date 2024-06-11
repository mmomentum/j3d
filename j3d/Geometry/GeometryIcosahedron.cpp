#include "GeometryIcosahedron.h"

namespace
{
	glm::vec3 normalizeWithSquareRoot(const glm::vec3& value, float radius)
	{
		float distance = glm::length(value);
		float normalizedDistance = std::sqrt(distance) / radius;
		return value / normalizedDistance;
	}
}// namespace


Mesh3D::Geometry GeometryIcosahedron::create(float radius, unsigned int detail)
{
	const float t = (1.0 + std::sqrt(5.0)) / 2.0; // Golden ratio

	std::vector<glm::vec3> vertices = {
		glm::vec3(-1,  t,  0), glm::vec3(1,  t,  0), glm::vec3(-1, -t,  0), glm::vec3(1, -t,  0),
		glm::vec3(0, -1,  t), glm::vec3(0,  1,  t), glm::vec3(0, -1, -t), glm::vec3(0,  1, -t),
		glm::vec3(t,  0, -1), glm::vec3(t,  0,  1), glm::vec3(-t,  0, -1), glm::vec3(-t,  0,  1)
	};

	for (int i = 0; i < vertices.size(); i++)
	{
		vertices[i] = glm::normalize(vertices[i]) * radius;
	}

	std::vector<int> indices = {
		0, 11, 5,   0, 5, 1,   0, 1, 7,   0, 7, 10,  0, 10, 11,
		1, 5, 9,    5, 11, 4,  11, 10, 2,  10, 7, 6,   7, 1, 8,
		3, 9, 4,    3, 4, 2,    3, 2, 6,   3, 6, 8,    3, 8, 9,
		4, 9, 5,    2, 4, 11,   6, 2, 10,  8, 6, 7,    9, 8, 1
	};

	// Subdivide the triangles to improve the detail
	for (int i = 0; i < detail; ++i)
	{
		std::vector<int> newIndices;

		for (size_t j = 0; j < indices.size(); j += 3)
		{
			int v1 = indices[j];
			int v2 = indices[j + 1];
			int v3 = indices[j + 2];

			glm::vec3 v12 = glm::normalize(vertices[v1] + vertices[v2]);
			glm::vec3 v23 = glm::normalize(vertices[v2] + vertices[v3]);
			glm::vec3 v31 = glm::normalize(vertices[v3] + vertices[v1]);

			int v12_idx = vertices.size();
			int v23_idx = v12_idx + 1;
			int v31_idx = v23_idx + 1;

			vertices.push_back(v12 * radius);
			vertices.push_back(v23 * radius);
			vertices.push_back(v31 * radius);

			newIndices.insert(newIndices.end(), { v1, v12_idx, v31_idx });
			newIndices.insert(newIndices.end(), { v2, v23_idx, v12_idx });
			newIndices.insert(newIndices.end(), { v3, v31_idx, v23_idx });
			newIndices.insert(newIndices.end(), { v12_idx, v23_idx, v31_idx });
		}

		indices = newIndices;
	}

	std::vector<glm::vec3> normals(vertices.size(), glm::vec3(0.0f));

	for (size_t i = 0; i < indices.size(); i += 3)
	{
		int i1 = indices[i];
		int i2 = indices[i + 1];
		int i3 = indices[i + 2];
		glm::vec3 edge1 = vertices[i2] - vertices[i1];
		glm::vec3 edge2 = vertices[i3] - vertices[i1];
		glm::vec3 faceNormal = glm::normalize(glm::cross(edge1, edge2));
		normals[i1] += faceNormal;
		normals[i2] += faceNormal;
		normals[i3] += faceNormal;
	}
	for (size_t i = 0; i < normals.size(); ++i)
	{
		normals[i] = glm::normalize(normals[i]);
	}

	// cylindrically map the UV coordinates
	std::vector<glm::vec2> textureCoords(vertices.size(), glm::vec2(0.0f));

	for (size_t i = 0; i < vertices.size(); ++i)
	{
		float theta = std::atan2(vertices[i].x, vertices[i].z);
		float phi = std::acos(vertices[i].y);
		float u = theta / (2 * juce::MathConstants<float>::pi) + 0.5f;
		float v = phi / juce::MathConstants<float>::pi;
		textureCoords[i] = glm::vec2(u, v);
	}

	// The Geometry struct will store the data and be returned
	Mesh3D::Geometry result;

	// for smooth normals, swap the commented out line with the current one
	// result.normals = vertices;
	result.normals = normals;
	result.vertices = vertices;
	result.textureCoords = textureCoords;
	result.indices = indices;

	return result;
}
