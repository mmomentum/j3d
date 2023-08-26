#pragma once

#include "../Mesh.h"

struct GeometryCylinder
{
    struct Params
    {
        float radiusTop = 1.0f;
        float radiusBottom = 1.0f;
        float height = 1.0f;
        unsigned int radialSegments = 16;
        unsigned int heightSegments = 1;
        bool openEnded = false;
        float thetaStart = 0;
        float thetaLength = juce::MathConstants<float>::twoPi;
    };

    static Mesh::Geometry create(Params& params);

    static Mesh::Geometry create(
        float radiusTop = 1,
        float radiusBottom = 1,
        float height = 1,
        unsigned int radialSegments = 16,
        unsigned int heightSegments = 1,
        bool openEnded = false,
        float thetaStart = 0,
        float thetaLength = juce::MathConstants<float>::twoPi);
};