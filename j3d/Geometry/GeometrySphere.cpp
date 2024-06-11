#include "GeometrySphere.h"

Mesh3D::Geometry GeometrySphere::create(Params& params)
{
	Mesh3D::Geometry g;

	unsigned int widthSegments = std::max(3u, params.widthSegments);
	unsigned int heightSegments = std::max(2u, params.heightSegments);

	const auto thetaEnd = std::min(params.thetaStart + params.thetaLength, juce::MathConstants<float>::pi);

	unsigned int index = 0;
	std::vector<std::vector<unsigned int>> grid;

	glm::vec3 vertex;
	glm::vec3 normal;

	// iterate through each height segment
	for (unsigned iy = 0; iy <= heightSegments; iy++)
	{
		std::vector<unsigned int> verticesRow;

		const float v = static_cast<float>(iy) / static_cast<float>(heightSegments);

		// special cases for the poles of the sphere
		float uOffset = 0;

		if (iy == 0 && params.thetaStart == 0)
		{
			uOffset = 0.5f / static_cast<float>(widthSegments);

		}
		else if (iy == heightSegments && thetaEnd == juce::MathConstants<float>::pi)
		{
			uOffset = -0.5f / static_cast<float>(widthSegments);
		}

		// for each width segment
		for (unsigned ix = 0; ix <= widthSegments; ix++)
		{
			const float u = static_cast<float>(ix) / static_cast<float>(widthSegments);

			// vertex

			vertex.x = -params.radius * std::cos(params.phiStart + u * params.phiLength) * std::sin(params.thetaStart + v * params.thetaLength);
			vertex.y = params.radius * std::cos(params.thetaStart + v * params.thetaLength);
			vertex.z = params.radius * std::sin(params.phiStart + u * params.phiLength) * std::sin(params.thetaStart + v * params.thetaLength);

			g.vertices.push_back({ vertex.x, vertex.y, vertex.z });

			// normal

			normal = glm::normalize(vertex);
			g.normals.push_back({ normal.x, normal.y, normal.z });

			// uv
			g.textureCoords.push_back({ u + uOffset, 1 - v });

			verticesRow.emplace_back(index++);
		}

		grid.emplace_back(verticesRow);
	}

	// create the indices
	for (int iy = 0; iy < heightSegments; iy++)
	{
		for (int ix = 0; ix < widthSegments; ix++)
		{
			const auto a = grid[iy][ix + 1];
			const auto b = grid[iy][ix];
			const auto c = grid[iy + 1][ix];
			const auto d = grid[iy + 1][ix + 1];

			if (iy != 0 || params.thetaStart > 0)
			{
				g.indices.push_back(a);
				g.indices.push_back(b);
				g.indices.push_back(d);
			}
			if (iy != heightSegments - 1 || thetaEnd < juce::MathConstants<float>::pi)
			{
				g.indices.push_back(b);
				g.indices.push_back(c);
				g.indices.push_back(d);
			}
		}
	}

	return g;
}

Mesh3D::Geometry GeometrySphere::create(float radius, unsigned int widthSegments,
	unsigned int heightSegments, float phiStart, float phiLength,
	float thetaStart, float thetaLength)
{
    auto params = Params{ radius, widthSegments, heightSegments, phiStart, phiLength, thetaStart, thetaLength };
    
	return create(params);
}
