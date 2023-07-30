#pragma once

#include "../Mesh.h"

struct GeometryTorus
{
	static Mesh::Geometry create(float radius = 1.0f, float tube = 0.4f,
		unsigned int radialSegments = 20, unsigned int tubularSegments = 64,
		float arc = juce::MathConstants<float>::twoPi);
};