#include "GeometryCube.h"

namespace
{
	struct CubeHelper
	{
		unsigned int numberOfVertices = 0;
		int groupStart = 0;

		std::list<unsigned int> indices;
		std::list<float> vertices;
		std::list<float> normals;
		std::list<float> uvs;

		void buildPlane(Mesh::Geometry& g, int u, int v, int w, float udir, float vdir,
			float width, float height, float depth, unsigned int gridX, unsigned int gridY,
			int materialIndex)
		{
			const auto segmentWidth = width / static_cast<float>(gridX);
			const auto segmentHeight = height / static_cast<float>(gridY);

			const auto widthHalf = width / 2;
			const auto heightHalf = height / 2;
			const auto depthHalf = depth / 2;

			const auto gridX1 = gridX + 1;
			const auto gridY1 = gridY + 1;

			int vertexCounter = 0;
			int groupCount = 0;

			auto vector = glm::vec3();

			// generate vertices, normals and uvs

			for (auto iy = 0; iy < gridY1; iy++) {

				const auto y = static_cast<float>(iy) * segmentHeight - heightHalf;

				for (auto ix = 0; ix < gridX1; ix++) {

					const auto x = static_cast<float>(ix) * segmentWidth - widthHalf;

					// set values to correct vector component

					vector[u] = x * udir;
					vector[v] = y * vdir;
					vector[w] = depthHalf;

					// now apply vector to vertex buffer

					g.vertices.push_back({ vector.x, vector.y, vector.z });

					// set values to correct vector component

					vector[u] = 0;
					vector[v] = 0;
					vector[w] = depth > 0 ? 1.f : -1.f;

					// now apply vector to normal buffer

					g.normals.push_back({ vector.x, vector.y, vector.z });

					// uvs

					g.textureCoords.push_back({ static_cast<float>(ix) / static_cast<float>(gridX),
						static_cast<float>(iy) / static_cast<float>(gridY) });

					// counters

					++vertexCounter;
				}
			}

			// indices

			// 1. you need three indices to draw a single face
			// 2. a single segment consists of two faces
			// 3. so we need to generate six (2*3) indices per segment

			for (auto iy = 0; iy < gridY; iy++) {

				for (auto ix = 0; ix < gridX; ix++) {

					const auto a = numberOfVertices + ix + gridX1 * iy;
					const auto b = numberOfVertices + ix + gridX1 * (iy + 1);
					const auto c = numberOfVertices + (ix + 1) + gridX1 * (iy + 1);
					const auto d = numberOfVertices + (ix + 1) + gridX1 * iy;

					// faces

					g.indices.push_back(a);
					g.indices.push_back(b);
					g.indices.push_back(d);

					g.indices.push_back(b);
					g.indices.push_back(c);
					g.indices.push_back(d);

					// increase counter

					groupCount += 6;
				}
			}

			// add a group to the geometry. this will ensure multi material support

			// g.addGroup(groupStart, groupCount, materialIndex);

			// calculate new start value for groups

			groupStart += groupCount;

			// update total number of vertices

			numberOfVertices += vertexCounter;
		}

		// generates the six faces of the cube using the buildPlane function
		CubeHelper(Mesh::Geometry& g, GeometryCube::Params& p)
		{
			buildPlane(g, 2, 1, 0, -1, -1, p.depth, p.height, p.width, p.depthSegments, p.heightSegments, 0); // px
			buildPlane(g, 2, 1, 0, 1, -1, p.depth, p.height, -p.width, p.depthSegments, p.heightSegments, 1); // nx
			buildPlane(g, 0, 2, 1, 1, 1, p.width, p.depth, p.height, p.widthSegments, p.depthSegments, 2);    // py
			buildPlane(g, 0, 2, 1, 1, -1, p.width, p.depth, -p.height, p.widthSegments, p.depthSegments, 3);  // ny
			buildPlane(g, 0, 1, 2, 1, -1, p.width, p.height, p.depth, p.widthSegments, p.heightSegments, 4);  // pz
			buildPlane(g, 0, 1, 2, -1, -1, p.width, p.height, -p.depth, p.widthSegments, p.heightSegments, 5);// nz
		}
	};
}

Mesh::Geometry GeometryCube::create(Params& params)
{
	Mesh::Geometry geometry;

	CubeHelper helper(geometry, params);

	return geometry;
}

Mesh::Geometry GeometryCube::create(float width, float height, float depth, unsigned int widthSegments, unsigned int heightSegments, unsigned int depthSegments)
{
	Mesh::Geometry geometry;

	CubeHelper helper(geometry, Params{ width, height, depth,
		widthSegments, heightSegments, depthSegments });

	return geometry;
}
