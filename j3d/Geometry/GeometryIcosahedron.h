#pragma once

#include "../Base/Mesh3D.h"

struct GeometryIcosahedron
{
	static Mesh3D::Geometry create(float radius, unsigned int detail);
};