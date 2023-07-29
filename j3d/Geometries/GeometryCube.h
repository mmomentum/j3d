#pragma once

#include "../Mesh.h"

// geometry generator for a cube (very basic)
struct GeometryCube
{
    struct Params 
    {
        float width;
        float height;
        float depth;

        unsigned int widthSegments;
        unsigned int heightSegments;
        unsigned int depthSegments;
    };

    const float width, height, depth;

    // creates a cube via the GeometryCube parameter set
    static Mesh::Geometry create(Params& params);

    // creates a cube with the values as arguments (or just the default values)
    static Mesh::Geometry create(float width = 1, float height = 1, float depth = 1, 
        unsigned int widthSegments = 1, unsigned int heightSegments = 1, 
        unsigned int depthSegments = 1);
};