#pragma once

#include "../Mesh.h"

#include "GeometryPolyhedron.h"

struct GeometryIcosahedron : public GeometryPolyhedron
{
	static Mesh::Geometry create(float radius, unsigned int detail);
};