#pragma once

#include "../Mesh.h"

struct GeometryIcosahedron
{
	static Mesh::Geometry create(float radius, unsigned int detail);
};