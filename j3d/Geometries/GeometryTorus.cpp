#include "GeometryTorus.h"

Mesh::Geometry GeometryTorus::create(float radius, float tube, unsigned int radialSegments, unsigned int tubularSegments, float arc)
{
	Mesh::Geometry g;

	glm::vec3 center;
	glm::vec3 vertex;
	glm::vec3 normal;

	for (unsigned j = 0; j <= radialSegments; j++)
	{
		for (unsigned i = 0; i <= tubularSegments; i++)
		{
			const auto u = static_cast<float>(i) / 
				static_cast<float>(tubularSegments) * arc;
			const auto v = static_cast<float>(j) /
				static_cast<float>(radialSegments) * juce::MathConstants<float>::twoPi;

			// vertex

			vertex.x = (radius + tube * std::cos(v)) * std::cos(u);
			vertex.y = (radius + tube * std::cos(v)) * std::sin(u);
			vertex.z = tube * std::sin(v);

			g.vertices.push_back({ vertex.x, vertex.y, vertex.z });

			// normal

			center.x = radius * std::cos(u);
			center.y = radius * std::sin(u);
			normal = glm::normalize(vertex - center);

			g.normals.push_back({ normal.x, normal.y, normal.z });

			// uv

			g.textureCoords.push_back({
				static_cast<float>(i) / static_cast<float>(tubularSegments),
				static_cast<float>(j) / static_cast<float>(radialSegments) });
		}
	}

	for (unsigned j = 1; j <= radialSegments; j++) 
	{
		for (unsigned i = 1; i <= tubularSegments; i++) 
		{
			// indices

			const auto a = (tubularSegments + 1) * j + i - 1;
			const auto b = (tubularSegments + 1) * (j - 1) + i - 1;
			const auto c = (tubularSegments + 1) * (j - 1) + i;
			const auto d = (tubularSegments + 1) * j + i;

			// faces

			g.indices.push_back(a);
			g.indices.push_back(b);
			g.indices.push_back(d);

			g.indices.push_back(b);
			g.indices.push_back(c);
			g.indices.push_back(d);
		}
	}

	return g;
}
