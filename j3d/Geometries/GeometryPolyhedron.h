#pragma once

#include "../Mesh.h"

struct GeometryPolyhedron
{
	GeometryPolyhedron(const std::vector<float>& vertices, const std::vector<unsigned int>& indices, 
		float radius, unsigned int detail);
};