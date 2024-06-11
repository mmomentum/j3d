#pragma once

#include "Common.h"
#include "WavefrontObjFile.h"

#include <assimp/mesh.h>

enum layoutPositions
{
	layout_vertex = 0,
	layout_normal = 1,
	layout_uv = 2
};

//This class assumes layout positions and formats that correspond to the sample shaders in Program.cpp
class Mesh3D
{
public:
	// alternate use datastructure that holds positions, normals, UVs and indices. they dont all 
	// *need* to be used though, this is mostly just here to make it so that primitive Mesh3D 
	// generators can pass all the info they might need to into the constructor
	struct Geometry
	{
		std::vector<glm::vec3> vertices, normals;
		std::vector<glm::vec2> textureCoords;
		std::vector<int> indices;
	};

	//Create a basic cube Mesh3D:
	Mesh3D();

	// creates a mesh with the given size of points, the positions of the points will be set later
	Mesh3D(int size);

	//Create a Mesh3D from raw data, needs to be triangles:
    //Mesh3D(std::vector<glm::vec3> positions);
	Mesh3D(std::vector<glm::vec3>& positions);
	Mesh3D(std::vector<glm::vec3>& positions, std::vector<glm::vec3>& normals);
	Mesh3D(std::vector<glm::vec3>& positions, std::vector<glm::vec3>& normals, std::vector<glm::vec2>& uvs);

	// loads a mesh from the geometry struct (primarily for primitive creation in the geometry folder)
	Mesh3D(Geometry& geometry);

	// loads a mesh from an assimp aiMesh. it's assumed that for multiple meshes you'd
	// handle implementing that in another higher level class
	Mesh3D(const aiMesh*, unsigned int shapeIdx = 0);

	// loads a Mesh3D from JUCE binary data. uses the internal parser & only supports obj
	Mesh3D(const char* binary_data, const int data_size, unsigned int shapeIdx = 0);

	~Mesh3D();

	void render();

	// renders without using any custom draw mode that was set; used primarily for mouse picking
	void renderSolid();

	const inline bool hasUVs() { return hasUVBuffer; }
	const inline bool hasNormals() { return hasNormalBuffer; }
	const inline bool isValid() { return valid; }

	void setCustomDrawMode(GLenum mode) { custom_gl_draw_mode = mode; }

	GLuint getVAO() { return VAO; }
	GLuint getBuffer(layoutPositions position) { return buffers[position]; }

protected:
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

	// for defining a custom GL draw mode to use (unindexed meshes only)
	GLenum custom_gl_draw_mode = GL_INVALID_ENUM;

	void initBuffer(const void* data, const int size, const int dimensions,
		const unsigned int layout, GLuint usage = GL_STATIC_DRAW);
};
