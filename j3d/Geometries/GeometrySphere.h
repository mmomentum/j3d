#pragma once

#include "../Mesh.h"

// generates a sphere. the difference between this one and the icosahedron is that this geometry is
// made to mimic the "UV Sphere" which is found in a lot of other 3d programs
struct GeometrySphere
{
	struct Params
	{
		float radius = 1.0f;
		unsigned int widthSegments = 16;
		unsigned int heightSegments = 12;
		float phiStart = 0;
		float phiLength = juce::MathConstants<float>::twoPi;
		float thetaStart = 0;
		float thetaLength = juce::MathConstants<float>::pi;
	};

	static Mesh::Geometry create(Params& params);

	static Mesh::Geometry create(
		float radius = 1,
		unsigned int widthSegments = 16,
		unsigned int heightSegments = 12,
		float phiStart = 0,
		float phiLength = juce::MathConstants<float>::twoPi,
		float thetaStart = 0,
		float thetaLength = juce::MathConstants<float>::pi);
};