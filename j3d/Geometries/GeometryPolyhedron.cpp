#include "GeometryPolyhedron.h"

namespace
{
	struct PolyhedronHelper
	{
		std::vector<float> vertexBuffer;
		std::vector<float> uvBuffer;

		PolyhedronHelper(const std::vector<float>& vertices,
			const std::vector<unsigned int>& indices,
			float radius, unsigned int detail) {

			subdivide(vertices, indices, detail);

			// all vertices should lie on a conceptual sphere with a given radius

			applyRadius(radius);

			// finally, create the uv data

			generateUVs();
		}

		void subdivide(const std::vector<float>& vertices, const std::vector<unsigned int>& indices,
			unsigned int detail)
		{
			glm::vec3 a;
			glm::vec3 b;
			glm::vec3 c;

			// iterate over all faces and apply a subdivision with the given detail value

			for (unsigned i = 0; i < indices.size(); i += 3) 
			{
				// get the vertices of the face

				getVertexByIndex(vertices, indices[i + 0], a);
				getVertexByIndex(vertices, indices[i + 1], b);
				getVertexByIndex(vertices, indices[i + 2], c);

				// perform subdivision

				subdivideFace(a, b, c, detail);
			}
		}

		void subdivideFace(const glm::vec3& a, const glm::vec3& b, const glm::vec3& c,
			unsigned int detail)
		{
			const auto cols = detail + 1;

			// we use this multidimensional array as a data structure for creating the subdivision

			std::vector<std::vector<glm::vec3>> v(cols + 1);

			// construct all of the vertices for this subdivision

			for (unsigned i = 0; i <= cols; i++)
			{
				auto& vi = v[i];

				auto aj = a + (c - a) * (static_cast<float>(i) / static_cast<float>(cols));
				auto bj = b + (c - b) * (static_cast<float>(i) / static_cast<float>(cols));

				const auto rows = cols - i;
				vi.resize(rows + 1);

				for (unsigned j = 0; j <= rows; j++)
				{
					if (j == 0 && i == cols)
					{
						vi[j] = aj;
					}
					else
					{
						vi[j] = aj + (bj - aj) * (static_cast<float>(j) / static_cast<float>(rows));
					}
				}
			}

			// construct all of the faces

			for (unsigned i = 0; i < cols; i++)
			{
				for (unsigned j = 0; j < 2 * (cols - i) - 1; j++)
				{
					const auto k = j / 2;

					if (j % 2 == 0)
					{
						pushVertex(v[i][k + 1]);
						pushVertex(v[i + 1][k]);
						pushVertex(v[i][k]);
					}
					else
					{
						pushVertex(v[i][k + 1]);
						pushVertex(v[i + 1][k + 1]);
						pushVertex(v[i + 1][k]);
					}
				}
			}
		}

		void applyRadius(float radius)
		{
			glm::vec3 vertex;

			// iterate over the entire buffer and apply the radius to each vertex

			for (unsigned i = 0; i < vertexBuffer.size(); i += 3)
			{
				vertex.x = vertexBuffer[i + 0];
				vertex.y = vertexBuffer[i + 1];
				vertex.z = vertexBuffer[i + 2];

				vertex = glm::normalize(vertex) * radius;

				vertexBuffer[i + 0] = vertex.x;
				vertexBuffer[i + 1] = vertex.y;
				vertexBuffer[i + 2] = vertex.z;
			}
		}

		void generateUVs()
		{
			glm::vec3 vertex;

			for (unsigned i = 0; i < vertexBuffer.size(); i += 3)
			{
				vertex.x = vertexBuffer[i + 0];
				vertex.y = vertexBuffer[i + 1];
				vertex.z = vertexBuffer[i + 2];

				const auto u = azimuth(vertex) / 2.f / juce::MathConstants<float>::pi + 0.5f;
				const auto v = inclination(vertex) / juce::MathConstants<float>::pi + 0.5f;
				uvBuffer.insert(uvBuffer.end(), { u, 1 - v });
			}

			correctUVs();

			correctSeam();
		}

		void correctSeam()
		{
			for (unsigned i = 0; i < uvBuffer.size(); i += 6)
			{
				// uv data of a single face

				const auto x0 = uvBuffer[i + 0];
				const auto x1 = uvBuffer[i + 2];
				const auto x2 = uvBuffer[i + 4];

				const auto max = std::max(std::max(x0, x1), x2);
				const auto min = std::min(std::min(x0, x1), x2);

				// 0.9 is somewhat arbitrary

				if (max > 0.9f && min < 0.1f) {

					if (x0 < 0.2) uvBuffer[i + 0] += 1;
					if (x1 < 0.2) uvBuffer[i + 2] += 1;
					if (x2 < 0.2) uvBuffer[i + 4] += 1;
				}
			}
		}

		void pushVertex(const glm::vec3& vertex) {

			vertexBuffer.emplace_back(vertex.x);
			vertexBuffer.emplace_back(vertex.y);
			vertexBuffer.emplace_back(vertex.z);
		}

		static void getVertexByIndex(const std::vector<float>& vertices, unsigned int index, glm::vec3& vertex) {

			const auto stride = index * 3;

			vertex.x = vertices[stride + 0];
			vertex.y = vertices[stride + 1];
			vertex.z = vertices[stride + 2];
		}

		void correctUVs() {

			glm::vec3 a;
			glm::vec3 b;
			glm::vec3 c;

			glm::vec3 centroid;

			glm::vec2 uvA;
			glm::vec2 uvB;
			glm::vec2 uvC;

			for (unsigned i = 0, j = 0; i < vertexBuffer.size(); i += 9, j += 6) 
			{
				a = glm::vec3(vertexBuffer[i + 0], vertexBuffer[i + 1], vertexBuffer[i + 2]);
				b = glm::vec3(vertexBuffer[i + 3], vertexBuffer[i + 4], vertexBuffer[i + 5]);
				c = glm::vec3(vertexBuffer[i + 6], vertexBuffer[i + 7], vertexBuffer[i + 8]);

				uvA = glm::vec2(uvBuffer[j + 0], uvBuffer[j + 1]);
				uvB = glm::vec2(uvBuffer[j + 2], uvBuffer[j + 3]);
				uvC = glm::vec2(uvBuffer[j + 4], uvBuffer[j + 5]);

				centroid = (a + b + c) / 3.0f;

				const auto azi = azimuth(centroid);

				correctUV(uvA, j + 0, a, azi);
				correctUV(uvB, j + 2, b, azi);
				correctUV(uvC, j + 4, c, azi);
			}
		}

		void correctUV(const glm::vec2& uv, unsigned int stride,
			const glm::vec3& vector, float azimuth)
		{
			if ((azimuth < 0) && (uv.x == 1))
			{
				uvBuffer[stride] = uv.x - 1;
			}

			if ((vector.x == 0) && (vector.z == 0))
			{
				uvBuffer[stride] = azimuth / 2.f / juce::MathConstants<float>::pi + 0.5f;
			}
		}

		// Angle around the Y axis, counter-clockwise when looking from above.

		static float azimuth(const glm::vec3& vector) {

			return std::atan2(vector.z, -vector.x);
		}


		// Angle above the XZ plane.

		static float inclination(const glm::vec3& vector) {

			return std::atan2(-vector.y, std::sqrt((vector.x * vector.x) + (vector.z * vector.z)));
		}
	};

}// namespace

GeometryPolyhedron::GeometryPolyhedron(const std::vector<float>& vertices, 
	const std::vector<unsigned int>& indices, float radius, unsigned int detail)
{
	PolyhedronHelper(vertices, indices, radius, detail);
}
