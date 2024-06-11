#pragma once

#include "../Base/Mesh3D.h"

// geometry generator for a cube (very basic)
struct GeometryCube
{
    struct Params 
    {
        float width = 1.0f;
        float height = 1.0f;
        float depth = 1.0f;

        unsigned int widthSegments = 1;
        unsigned int heightSegments = 1;
        unsigned int depthSegments = 1;
    };

    const float width, height, depth;

    // creates a cube via the GeometryCube parameter set
    static Mesh3D::Geometry create(Params& params);

    // creates a cube with the values as arguments (or just the default values)
    static Mesh3D::Geometry create(float width = 1, float height = 1, float depth = 1, 
        unsigned int widthSegments = 1, unsigned int heightSegments = 1, 
        unsigned int depthSegments = 1);
};