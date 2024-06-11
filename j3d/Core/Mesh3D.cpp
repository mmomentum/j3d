#include "Mesh3D.h"

void Mesh3D::initBuffer(const void* data, const int size, const int dimensions, const unsigned int layout, GLuint usage)
{
	//Pass data to graphics card:
	glBindBuffer(GL_ARRAY_BUFFER, buffers[layout]);
	glBufferData(GL_ARRAY_BUFFER, size, data, usage);

	//Set up buffer meta-data:
	glEnableVertexAttribArray(layout);
	glVertexAttribPointer(layout, dimensions, GL_FLOAT, GL_FALSE, 0, (void*)0);
}

Mesh3D::Mesh3D(int size)
{
	if (size < 1)
	{
		DBG("Passed empty vector to Mesh3D constructor.");
		return;
	}

	auto tmp_points = std::vector<glm::vec3>(size);

	for (int i = 0; i < size; i++)
	{
		tmp_points[i] = glm::vec3(0.0f, 0.0f, 0.0f);
	}

	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, buffers);

	glBindVertexArray(VAO);

	numVerts = tmp_points.size();
	initBuffer(&tmp_points[0][0], tmp_points.size() * sizeof(glm::vec3), 3, layout_vertex);

	glBindVertexArray(0);

	//If false, this was either not called on the main thread, or you did not initalize OpenGL, or you ran out of memory or something:
	if (glIsVertexArray(VAO))
		valid = true;
	else
		DBG("Error creating vertex array object for Mesh3D.");
}

Mesh3D::Mesh3D(std::vector<glm::vec3>& positions)
{
	if (positions.size() < 1)
	{
		DBG("Passed empty vector to Mesh3D constructor.");
		return;
	}

	//Allocate handles for the Mesh3D in OpenGL:
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, buffers);

	glBindVertexArray(VAO);

	numVerts = positions.size();
	initBuffer(&positions[0][0], positions.size() * sizeof(glm::vec3), 3, layout_vertex);

	glBindVertexArray(0);

	//If false, this was either not called on the main thread, or you did not initalize OpenGL, or you ran out of memory or something:
	if (glIsVertexArray(VAO))
		valid = true;
	else
		DBG("Error creating vertex array object for Mesh3D.");
}

Mesh3D::Mesh3D(std::vector<glm::vec3>& positions, std::vector<glm::vec3>& normals)
{
	if (positions.size() < 1)
	{
		DBG("Passed empty vector to Mesh3D constructor.");
		return;
	}
	if (positions.size() != normals.size())
	{
		DBG("Number of positions not equal to number of normals for Mesh3D!");
		return;
	}

	//Allocate handles for the Mesh3D in OpenGL:
	glGenVertexArrays(1, &VAO);
	glGenBuffers(2, buffers);

	glBindVertexArray(VAO);

	numVerts = positions.size();
	initBuffer(&positions[0][0], positions.size() * sizeof(glm::vec3), 3, layout_vertex);

	initBuffer(&normals[0][0], normals.size() * sizeof(glm::vec3), 3, layout_normal);

	glBindVertexArray(0);

	//If false, this was either not called on the main thread, or you did not initalize OpenGL, or you ran out of memory or something:
	if (glIsVertexArray(VAO))
		valid = true;
	else
		DBG("Error creating vertex array object for Mesh3D.");
}

Mesh3D::Mesh3D(std::vector<glm::vec3>& positions, std::vector<glm::vec3>& normals, std::vector<glm::vec2>& uvs)
{
	if (positions.size() < 1)
	{
		DBG("Passed empty vector to Mesh3D constructor.");
		return;
	}
	if (positions.size() != normals.size())
	{
		DBG("Number of positions not equal to number of normals for Mesh3D!");
		return;
	}
	if (positions.size() != uvs.size())
	{
		DBG("Number of positions not equal to number of uvs for Mesh3D!");
		return;
	}

	//Allocate handles for the Mesh3D in OpenGL:
	glGenVertexArrays(1, &VAO);
	glGenBuffers(3, buffers);

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
		DBG("Error creating vertex array object for Mesh3D.");
}

Mesh3D::Mesh3D(Geometry& g)
{
	glGenVertexArrays(1, &VAO);
	glGenBuffers(3, buffers);

	glBindVertexArray(VAO);

	initBuffer(g.vertices.data(), g.vertices.size() * sizeof(glm::vec3), 3, layout_vertex);

	if (g.normals.size() > 0)
	{
		initBuffer(g.normals.data(), g.normals.size() * sizeof(glm::vec3), 3, layout_normal);
		hasNormalBuffer = true;
	}

	if (g.textureCoords.size() > 0)
	{
		initBuffer(g.textureCoords.data(), g.textureCoords.size() * sizeof(glm::vec2), 2, layout_uv);
		hasUVBuffer = true;
	}

	//Indices:
	if (g.indices.size() > 0)
	{
		glGenBuffers(1, &indexBuffer);

		numVerts = g.indices.size();
		isIndexed = true;
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, g.indices.size() * sizeof(int), g.indices.data(), GL_STATIC_DRAW);
	}
	else
	{
		numVerts = g.vertices.size();
	}

	glBindVertexArray(0);

	if (glIsVertexArray(VAO))
		valid = true;
	else
		DBG("Error creating vertex array object for Mesh3D.");
}

Mesh3D::Mesh3D()
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
		cubeUVs.push_back({ cubeVerts[a].x > 0 ? 1.0 : 0.0,cubeVerts[a].z > 0 ? 1.0 : 0.0 });

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

	//Allocate handles for the Mesh3D in OpenGL:
	glGenVertexArrays(1, &VAO);
	glGenBuffers(3, buffers);

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
		DBG("Error creating vertex array object for Mesh3D.");
}

Mesh3D::Mesh3D(const aiMesh* mesh, unsigned int shapeIdx)
{
	Geometry g;

	int numBuffers = 1;

	// Iterate through the vertices
	for (unsigned int i = 0; i < mesh->mNumVertices; ++i)
	{
		// Vertices
		glm::vec3 vertex;
		vertex.x = mesh->mVertices[i].x;
		vertex.y = mesh->mVertices[i].y;
		vertex.z = mesh->mVertices[i].z;
		g.vertices.push_back(vertex);

		// Normals (if available)
		if (mesh->HasNormals())
		{
			if (i == 0)
				numBuffers++;

			glm::vec3 normal;
			normal.x = mesh->mNormals[i].x;
			normal.y = mesh->mNormals[i].y;
			normal.z = mesh->mNormals[i].z;
			g.normals.push_back(normal);
		}

		// Texture coordinates (if available)
		if (mesh->HasTextureCoords(0))
		{
			if (i == 0)
				numBuffers++;

			glm::vec2 texCoord;
			texCoord.x = mesh->mTextureCoords[0][i].x;
			texCoord.y = mesh->mTextureCoords[0][i].y;
			g.textureCoords.push_back(texCoord);
		}
	}

	// Iterate through the faces (indices)
	for (unsigned int i = 0; i < mesh->mNumFaces; ++i)
	{
		const aiFace& face = mesh->mFaces[i];

		for (unsigned int j = 0; j < face.mNumIndices; ++j)
		{
			g.indices.push_back(face.mIndices[j]);
		}
	}

	glGenVertexArrays(1, &VAO);
	glGenBuffers(numBuffers, buffers);

	glBindVertexArray(VAO);

	numVerts = g.vertices.size();

	if (g.vertices.size() != 0)
		initBuffer(g.vertices.data(), g.vertices.size() * sizeof(glm::vec3), 3, layout_vertex);

	if (g.normals.size() != 0)
		initBuffer(g.normals.data(), g.normals.size() * sizeof(glm::vec3), 3, layout_normal);

	if (g.textureCoords.size() != 0)
		initBuffer(g.textureCoords.data(), g.textureCoords.size() * sizeof(glm::vec2), 2, layout_uv);

	glBindVertexArray(0);

	//If false, this was either not called on the main thread, or you did not initalize OpenGL, or you ran out of memory or something:
	if (glIsVertexArray(VAO))
		valid = true;
	else
		DBG("Error creating vertex array object for Mesh3D.");
}

Mesh3D::Mesh3D(const char* binary_data, const int data_size, unsigned int shapeIdx) : VAO(0)
{
	WavefrontObjFile source;
	source.load(binary_data, data_size);

	if (source.shapes.size() < 1)
	{
		DBG("Could not load model file from binary data Mesh3D.");
		return;
	}

	if (shapeIdx >= source.shapes.size())
	{
		shapeIdx = source.shapes.size() - 1;
		DBG("shapeIdx out of range for binary data Mesh3D.");
		return;
	}

	WavefrontObjFile::Mesh* sourceMesh3D = &source.shapes[shapeIdx]->mesh;

	if (sourceMesh3D->vertices.size() < 1)
	{
		DBG("No vertices in shape for binary data Mesh3D");
		return;
	}

	if (sourceMesh3D->indices.size() < 1)
	{
		DBG("No indicies in shape for binary data Mesh3D");
		return;
	}

	//TODO: Add checks for normals, uvs...
	glGenVertexArrays(1, &VAO);
	glGenBuffers(3, buffers);
	glGenBuffers(1, &indexBuffer);
    
	glBindVertexArray(VAO);

	//Pass position data:
	initBuffer(sourceMesh3D->vertices.data(), sourceMesh3D->vertices.size() * sizeof(WavefrontObjFile::Vertex), 3, layout_vertex);

	if (sourceMesh3D->normals.size() > 0)
	{
		initBuffer(sourceMesh3D->normals.data(), sourceMesh3D->normals.size() * sizeof(WavefrontObjFile::Vertex), 3, layout_normal);
		hasNormalBuffer = true;
	}

	if (sourceMesh3D->textureCoords.size() > 0)
	{
		initBuffer(sourceMesh3D->textureCoords.data(), sourceMesh3D->textureCoords.size() * sizeof(WavefrontObjFile::TextureCoord), 2, layout_uv);
		hasUVBuffer = true;
	}

	//Indices:
	numVerts = sourceMesh3D->indices.size();
	isIndexed = true;
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sourceMesh3D->indices.size() * sizeof(WavefrontObjFile::Index), sourceMesh3D->indices.data(), GL_STATIC_DRAW);

	glBindVertexArray(0);

	valid = true;
}

Mesh3D::~Mesh3D()
{
	if (valid)
	{
		// Delete the normal buffer if it exists
		if (hasNormalBuffer)
		{
			glDeleteBuffers(1, &buffers[layout_normal]);
		}

		// Delete the UV buffer if it exists
		if (hasUVBuffer)
		{
			glDeleteBuffers(1, &buffers[layout_uv]);
		}

		// Delete the vertex buffer
		glDeleteBuffers(1, &buffers[layout_vertex]);

		if (isIndexed)
		{
			glDeleteVertexArrays(1, &indexBuffer);
		}
		else
		{
			glDeleteVertexArrays(1, &VAO);
		}

		// Reset valid flag to false to indicate that resources have been released
		valid = false;
	}
}

void Mesh3D::render()
{
	if (!valid)
		return;

	glBindVertexArray(VAO);

	if (isIndexed)
	{
		if (custom_gl_draw_mode == GL_INVALID_ENUM)
			glDrawElements(GL_TRIANGLES, numVerts, GL_UNSIGNED_INT, (void*)0);
		else
			glDrawElements(custom_gl_draw_mode, numVerts, GL_UNSIGNED_INT, (void*)0);
	}
	else
	{
		if (custom_gl_draw_mode == GL_INVALID_ENUM)
			glDrawArrays(GL_TRIANGLES, 0, numVerts);
		else
			glDrawArrays(custom_gl_draw_mode, 0, numVerts);
	}

	glBindVertexArray(0);
}

void Mesh3D::renderSolid()
{
	if (!valid)
		return;

	glBindVertexArray(VAO);

	if (isIndexed)
	{
		glDrawElements(GL_TRIANGLES, numVerts, GL_UNSIGNED_INT, (void*)0);
	}
	else
	{
		glDrawArrays(GL_TRIANGLES, 0, numVerts);
	}
}
