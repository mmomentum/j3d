/*
  ==============================================================================

    Material.h
    Created: 8 Jul 2023 1:34:27am
    Author:  Syerjchep

  ==============================================================================
*/

#pragma once

#include "Common.h"
#include "Texture.h"

class Material
{
    private:
        bool useWireframe = false;
        //Albedo, normal map, mohr:
        Texture *textures[3];
        bool textureUsed[3] = { false,false,false };
        glm::vec3 clipPlaneDirection = glm::vec3(1.0, 1.0, 0);
        float clipOffset = 0.5;
        bool doClipping = false;

    public:

        void enableClipping(glm::vec3 axis, float offset);
        inline void disableClipping() { doClipping = false; }
        void setTexture(Texture* in, textureBindNames type);
        inline void wireframeMode(bool value) { useWireframe = value; }
        const void render(program *boundProgram);
};