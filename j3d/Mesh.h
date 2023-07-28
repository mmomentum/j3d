/*
  ==============================================================================

    Mesh.h
    Created: 8 Jul 2023 8:42:35pm
    Author:  Syerjchep

  ==============================================================================
*/

#pragma once

#include "Common.h"
#include "../j3d/WavefrontObjFile.h"

enum layoutPositions 
{ 
    layout_vertex=0,
    layout_normal = 1,
    layout_uv=2
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

        //Create a basic cube mesh:
        Mesh();
        
        //Create a mesh from raw data, needs to be triangles:
        Mesh(std::vector<glm::vec3> &positions);
        Mesh(std::vector<glm::vec3> &positions,std::vector<glm::vec3> &normals);
        Mesh(std::vector<glm::vec3> &positions, std::vector<glm::vec3> &normals,std::vector<glm::vec2> &uvs);

        //Load a mesh from a file using default juce wavefront obj loader
        //It's just a mesh, not a whole model, so shapeIdx can be used to select which mesh if a file contains multiple
        Mesh(const char* filePath, unsigned int shapeIdx = 0);

        void render();

        const inline bool hasUVs() { return hasUVBuffer; }
        const inline bool hasNormals() { return hasNormalBuffer; }
        const inline bool isValid() { return valid; }

        ~Mesh();
};
