#include "GeometryCylinder.h"

Mesh::Geometry GeometryCylinder::create(Params& p)
{
	Mesh::Geometry g;

	unsigned int index = 0;
	const float halfHeight = p.height / 2;
	std::vector<std::vector<unsigned int>> indexArray;

	glm::vec3 normal, vertex;

	const float slope = (p.radiusBottom - p.radiusTop) / p.height;

    for (unsigned y = 0; y <= p.heightSegments; y++) 
    {
        std::vector<unsigned int> indexRow;

        const auto v = static_cast<float>(y) / static_cast<float>(p.heightSegments);

        // calculate the radius of the current row

        const auto radius = v * (p.radiusBottom - p.radiusTop) + p.radiusTop;

        for (unsigned x = 0; x <= p.radialSegments; x++)
        {
            const auto u = static_cast<float>(x) / static_cast<float>(p.radialSegments);

            const auto theta = u * p.thetaLength + p.thetaStart;

            const auto sinTheta = std::sin(theta);
            const auto cosTheta = std::cos(theta);

            // vertex

            vertex.x = radius * sinTheta;
            vertex.y = -v * p.height + halfHeight;
            vertex.z = radius * cosTheta;
            g.vertices.insert(g.vertices.end(), { vertex.x, vertex.y, vertex.z });

            // normal

            normal = glm::normalize(glm::vec3(sinTheta, slope, cosTheta));
            g.normals.insert(g.normals.end(), { normal.x, normal.y, normal.z });

            // uv

            g.textureCoords.insert(g.textureCoords.end(), { u, 1 - v });

            // save index of vertex in respective row

            indexRow.emplace_back(index++);
        }

        // now save vertices of the row in our index array

        indexArray.emplace_back(indexRow);
    }

    for (unsigned x = 0; x < p.radialSegments; x++) 
    {
        for (unsigned y = 0; y < p.heightSegments; y++)
        {

            // we use the index array to access the correct indices

            const auto a = indexArray[y][x];
            const auto b = indexArray[y + 1][x];
            const auto c = indexArray[y + 1][x + 1];
            const auto d = indexArray[y][x + 1];

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

Mesh::Geometry GeometryCylinder::create(float rTop, float rBottom, float h, unsigned int rSegments, unsigned int hSegments, bool open, float tStart, float tLength)
{
    return create(Params{ rTop, rBottom, h, rSegments, hSegments, open, tStart, tLength });
}
