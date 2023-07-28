/*
  ==============================================================================

    Mesh.cpp
    Created: 8 Jul 2023 8:42:33pm
    Author:  Syerjchep

  ==============================================================================
*/

#include "Mesh.h"

void Mesh::initBuffer(const void* data, const int size, const int dimensions, const unsigned int layout)
{
    //Pass data to graphics card:
    glBindBuffer(GL_ARRAY_BUFFER, buffers[layout]);
    glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW);

    //Set up buffer meta-data:
    glEnableVertexAttribArray(layout);
    glVertexAttribPointer(layout, dimensions, GL_FLOAT, GL_FALSE, 0, (void*)0);
}

Mesh::Mesh(const char* filePath,unsigned int shapeIdx)
{
    WavefrontObjFile source;
    source.load(juce::File(filePath));

    if (source.shapes.size() < 1)
    {
        DBG("Could not load model file " + std::string(filePath));
        return;
    }

    if (shapeIdx >= source.shapes.size())
    {
        shapeIdx = source.shapes.size() - 1;
        DBG("shapeIdx out of range for " + std::string(filePath));
        return;
    }

    WavefrontObjFile::Mesh *sourceMesh = &source.shapes[shapeIdx]->mesh;

    if (sourceMesh->vertices.size() < 1)
    {
        DBG("No vertices in shape for file " + std::string(filePath) + "!");
        return;
    }

    if (sourceMesh->indices.size() < 1)
    {
        DBG("No indicies in shape for file " + std::string(filePath) + "!");
        return;
    }

    //TODO: Add checks for normals, uvs...
    glCreateVertexArrays(1, &VAO);
    glCreateBuffers(3, buffers);
    glCreateBuffers(1, &indexBuffer);

    glBindVertexArray(VAO);

    //Pass position data:
    initBuffer(sourceMesh->vertices.data(), sourceMesh->vertices.size() * sizeof(WavefrontObjFile::Vertex), 3, layout_vertex);

    if (sourceMesh->normals.size() > 0)
    {
        initBuffer(sourceMesh->normals.data(), sourceMesh->normals.size() * sizeof(WavefrontObjFile::Vertex), 3, layout_normal);
        hasNormalBuffer = true;
    }

    if (sourceMesh->textureCoords.size() > 0)
    {
        initBuffer(sourceMesh->textureCoords.data(), sourceMesh->textureCoords.size() * sizeof(WavefrontObjFile::TextureCoord), 2, layout_uv);
        hasUVBuffer = true;
    }

    //Indices:
    numVerts = sourceMesh->indices.size();
    isIndexed = true;
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sourceMesh->indices.size() * sizeof(WavefrontObjFile::Index), sourceMesh->indices.data(), GL_STATIC_DRAW);

    glBindVertexArray(0);

    valid = true;
}

Mesh::Mesh(std::vector<glm::vec3> &positions)
{
    if (positions.size() < 1)
    {
        DBG("Passed empty vector to Mesh constructor.");
        return;
    }

    //Allocate handles for the mesh in OpenGL:
    glCreateVertexArrays(1, &VAO);
    glCreateBuffers(1, buffers);

    glBindVertexArray(VAO);

    numVerts = positions.size();
    initBuffer(&positions[0][0], positions.size() * sizeof(glm::vec3), 3, layout_vertex);

    glBindVertexArray(0);

    //If false, this was either not called on the main thread, or you did not initalize OpenGL, or you ran out of memory or something:
    if (glIsVertexArray(VAO))
        valid = true;
    else
        DBG("Error creating vertex array object for Mesh.");
}

Mesh::Mesh(std::vector<glm::vec3>& positions,std::vector<glm::vec3> &normals)
{
    if (positions.size() < 1)
    {
        DBG("Passed empty vector to Mesh constructor.");
        return;
    }
    if (positions.size() != normals.size())
    {
        DBG("Number of positions not equal to number of normals for mesh!");
        return;
    }

    //Allocate handles for the mesh in OpenGL:
    glCreateVertexArrays(1, &VAO);
    glCreateBuffers(2, buffers);

    glBindVertexArray(VAO);

    numVerts = positions.size();
    initBuffer(&positions[0][0], positions.size() * sizeof(glm::vec3), 3, layout_vertex);

    initBuffer(&normals[0][0], normals.size() * sizeof(glm::vec3), 3, layout_normal);

    glBindVertexArray(0);

    //If false, this was either not called on the main thread, or you did not initalize OpenGL, or you ran out of memory or something:
    if (glIsVertexArray(VAO))
        valid = true;
    else
        DBG("Error creating vertex array object for Mesh.");
}

Mesh::Mesh(std::vector<glm::vec3>& positions, std::vector<glm::vec3>& normals, std::vector<glm::vec2> &uvs)
{
    if (positions.size() < 1)
    {
        DBG("Passed empty vector to Mesh constructor.");
        return;
    }
    if (positions.size() != normals.size())
    {
        DBG("Number of positions not equal to number of normals for mesh!");
        return;
    }
    if (positions.size() != uvs.size())
    {
        DBG("Number of positions not equal to number of uvs for mesh!");
        return;
    }

    //Allocate handles for the mesh in OpenGL:
    glCreateVertexArrays(1, &VAO);
    glCreateBuffers(3, buffers);

    glBindVertexArray(VAO);

    numVerts = positions.size();
    initBuffer(&positions[0][0], positions.size() * sizeof(glm::vec3), 3, layout_vertex);

    initBuffer(&normals[0][0], normals.size() * sizeof(glm::vec3), 3, layout_normal);

    initBuffer(&uvs[0][0], uvs.size() * sizeof(glm::vec2), 2, layout_uv);

    glBindVertexArray(0);

    //If false, this was either not called on the main thread, or you did not initalize OpenGL, or you ran out of memory or something:
    if (glIsVertexArray(VAO))
        valid = true;
    else
        DBG("Error creating vertex array object for Mesh.");
}

Mesh::Mesh()
{
    float adjust = 0.5;

    std::vector<glm::vec3> cubeVerts;
    std::vector<glm::vec3> cubeNormals;
    std::vector<glm::vec2> cubeUVs;

    //Top
    cubeVerts.push_back(glm::vec3(adjust, adjust, -adjust));
    cubeVerts.push_back(glm::vec3(-adjust, adjust, -adjust));
    cubeVerts.push_back(glm::vec3(adjust, adjust, adjust));
    cubeVerts.push_back(glm::vec3(-adjust, adjust, adjust));
    cubeVerts.push_back(glm::vec3(adjust, adjust, adjust));
    cubeVerts.push_back(glm::vec3(-adjust, adjust, -adjust));

    for (int a = 0; a < 6; a++)
        cubeNormals.push_back({ 0,1,0 });

    for (int a = 0; a < 6; a++)
        cubeUVs.push_back({cubeVerts[a].x > 0 ? 1.0 : 0.0,cubeVerts[a].z > 0 ? 1.0 : 0.0});

    //Right
    cubeVerts.push_back(glm::vec3(adjust, -adjust, adjust));
    cubeVerts.push_back(glm::vec3(adjust, -adjust, -adjust));
    cubeVerts.push_back(glm::vec3(adjust, adjust, adjust));
    cubeVerts.push_back(glm::vec3(adjust, adjust, -adjust));
    cubeVerts.push_back(glm::vec3(adjust, adjust, adjust));
    cubeVerts.push_back(glm::vec3(adjust, -adjust, -adjust));

    for (int a = 0; a < 6; a++)
        cubeNormals.push_back({ 1,0,0 });

    for (int a = 6; a < 12; a++)
        cubeUVs.push_back({ cubeVerts[a].y > 0 ? 1.0 : 0.0,cubeVerts[a].z > 0 ? 1.0 : 0.0 });

    //Left
    cubeVerts.push_back(glm::vec3(-adjust, -adjust, adjust));
    cubeVerts.push_back(glm::vec3(-adjust, adjust, adjust));
    cubeVerts.push_back(glm::vec3(-adjust, -adjust, -adjust));
    cubeVerts.push_back(glm::vec3(-adjust, -adjust, -adjust));
    cubeVerts.push_back(glm::vec3(-adjust, adjust, adjust));
    cubeVerts.push_back(glm::vec3(-adjust, adjust, -adjust));

    for (int a = 0; a < 6; a++)
        cubeNormals.push_back({ -1,0,0 });

    for (int a = 12; a < 18; a++)
        cubeUVs.push_back({ cubeVerts[a].y > 0 ? 1.0 : 0.0,cubeVerts[a].z > 0 ? 1.0 : 0.0 });

    //Front
    cubeVerts.push_back(glm::vec3(-adjust, adjust, adjust));
    cubeVerts.push_back(glm::vec3(-adjust, -adjust, adjust));
    cubeVerts.push_back(glm::vec3(adjust, adjust, adjust));
    cubeVerts.push_back(glm::vec3(adjust, -adjust, adjust));
    cubeVerts.push_back(glm::vec3(adjust, adjust, adjust));
    cubeVerts.push_back(glm::vec3(-adjust, -adjust, adjust));

    for (int a = 0; a < 6; a++)
        cubeNormals.push_back({ 0,0,1 });

    for (int a = 18; a < 24; a++)
        cubeUVs.push_back({ cubeVerts[a].x > 0 ? 1.0 : 0.0,cubeVerts[a].y > 0 ? 1.0 : 0.0 });

    //Back
    cubeVerts.push_back(glm::vec3(-adjust, adjust, -adjust));
    cubeVerts.push_back(glm::vec3(adjust, adjust, -adjust));
    cubeVerts.push_back(glm::vec3(-adjust, -adjust, -adjust));
    cubeVerts.push_back(glm::vec3(-adjust, -adjust, -adjust));
    cubeVerts.push_back(glm::vec3(adjust, adjust, -adjust));
    cubeVerts.push_back(glm::vec3(adjust, -adjust, -adjust));

    for (int a = 0; a < 6; a++)
        cubeNormals.push_back({ 0,0,-1 });

    for (int a = 24; a < 30; a++)
        cubeUVs.push_back({ cubeVerts[a].x > 0 ? 1.0 : 0.0,cubeVerts[a].y > 0 ? 1.0 : 0.0 });

    //Bottom
    cubeVerts.push_back(glm::vec3(-adjust, -adjust, -adjust));
    cubeVerts.push_back(glm::vec3(adjust, -adjust, -adjust));
    cubeVerts.push_back(glm::vec3(adjust, -adjust, adjust));
    cubeVerts.push_back(glm::vec3(adjust, -adjust, adjust));
    cubeVerts.push_back(glm::vec3(-adjust, -adjust, adjust));
    cubeVerts.push_back(glm::vec3(-adjust, -adjust, -adjust));

    for (int a = 0; a < 6; a++)
        cubeNormals.push_back({ 0,-1,0 });

    for (int a = 30; a < 36; a++)
        cubeUVs.push_back({ cubeVerts[a].x > 0 ? 1.0 : 0.0,cubeVerts[a].z > 0 ? 1.0 : 0.0 });

    //Allocate handles for the mesh in OpenGL:
    glCreateVertexArrays(1, &VAO);
    glCreateBuffers(3, buffers);

    glBindVertexArray(VAO);

    numVerts = cubeVerts.size();
    initBuffer(&cubeVerts[0][0], cubeVerts.size() * sizeof(glm::vec3), 3, layout_vertex);
    initBuffer(&cubeNormals[0][0], cubeNormals.size() * sizeof(glm::vec3), 3, layout_normal);
    initBuffer(&cubeUVs[0][0], cubeUVs.size() * sizeof(glm::vec2), 2, layout_uv);

    glBindVertexArray(0);

    //If false, this was either not called on the main thread, or you did not initalize OpenGL, or you ran out of memory or something:
    if (glIsVertexArray(VAO))
        valid = true;
    else
        DBG("Error creating vertex array object for Mesh.");
}

void Mesh::render()
{
    if (!valid)
        return;

    glBindVertexArray(VAO);
    if (isIndexed)
        glDrawElements(GL_TRIANGLES, numVerts, GL_UNSIGNED_INT, (void*)0);
    else
        glDrawArrays(GL_TRIANGLES, 0, numVerts);
    glBindVertexArray(0);
}
