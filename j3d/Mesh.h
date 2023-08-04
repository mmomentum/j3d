#pragma once

#include "Common.h"
#include "../j3d/WavefrontObjFile.h"

enum layoutPositions
{
	layout_vertex = 0,
	layout_normal = 1,
	layout_uv = 2
};

//This class assumes layout positions and formats that correspond to the sample shaders in Program.cpp
class Mesh
{
private:
	GLuint VAO = 0;
	//Vertex, normal, uv:
	GLuint buffers[3] = { 0,0,0 };
	//Special index buffer:
	GLuint indexBuffer;

	int numVerts = 0;
	bool hasNormalBuffer = false;
	bool hasUVBuffer = false;
	bool valid = false;
	bool isIndexed = false;

	void initBuffer(const void* data, const int size, const int dimensions, const unsigned int layout);

public:
	// alternate use datastructure that holds positions, normals, UVs and indices. they dont all 
	// *need* to be used though, this is mostly just here to make it so that primitive mesh 
	// generators can pass all the info they might need to into the constructor
	struct Geometry
	{
		std::vector<glm::vec3> vertices, normals;
		std::vector<glm::vec2> textureCoords;
		std::vector<int> indices;
	};

	//Create a basic cube mesh:
	Mesh();

	//Create a mesh from raw data, needs to be triangles:
	Mesh(std::vector<glm::vec3>& positions);
	Mesh(std::vector<glm::vec3>& positions, std::vector<glm::vec3>& normals);
	Mesh(std::vector<glm::vec3>& positions, std::vector<glm::vec3>& normals, std::vector<glm::vec2>& uvs);
	Mesh(Geometry& geometry);

	// Load a mesh from a file using default juce wavefront obj loader
	// It's just a mesh, not a whole model, so shapeIdx can be used to select which mesh if a file contains multiple
	Mesh(juce::File filePath, unsigned int shapeIdx = 0);

	// loads a mesh from JUCE binary data
	Mesh(const char* binary_data, const int data_size, unsigned int shapeIdx = 0);

	void render();

	const inline bool hasUVs() { return hasUVBuffer; }
	const inline bool hasNormals() { return hasNormalBuffer; }
	const inline bool isValid() { return valid; }

	~Mesh() {};
};
