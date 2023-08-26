#pragma once

#include "../Mesh.h"

#include "GeometryCylinder.h"

struct GeometryCone : public GeometryCylinder
{
    struct Params
    {
        float radius = 1.0f;
        float height = 1.0f;
        unsigned int radialSegments = 16;
        unsigned int heightSegments = 1;
        bool openEnded = false;
        float thetaStart = 0;
        float thetaLength = juce::MathConstants<float>::twoPi;
    };

    static Mesh::Geometry create(Params& params);

    static Mesh::Geometry create(
        float radius = 1,
        float height = 1,
        unsigned int radialSegments = 16,
        unsigned int heightSegments = 1,
        bool openEnded = false,
        float thetaStart = 0,
        float thetaLength = juce::MathConstants<float>::twoPi);
};