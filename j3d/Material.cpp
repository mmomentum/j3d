/*
  ==============================================================================

    Material.cpp
    Created: 8 Jul 2023 1:34:23am
    Author:  Syerjchep

  ==============================================================================
*/

#include "Material.h"

void Material::enableClipping(glm::vec3 axis, float offset)
{ 
    doClipping = true; 
    clipPlaneDirection = glm::normalize(axis); 
    clipOffset = offset; 
}

void Material::setTexture(Texture* in, textureBindNames type)
{
    if (type == textureBindNames::endOfEnum)
        return;

    if (in)
    {
        textures[type] = in;
        textureUsed[type] = true;
    }
    else
        textureUsed[type] = false;
}

const void Material::render(program* boundProgram)
{
    if (doClipping)
    {
        glUniform3f(boundProgram->uniform_clipPlaneDirection, clipPlaneDirection.x, clipPlaneDirection.y, clipPlaneDirection.z);
        glUniform1f(boundProgram->uniform_clipPlaneOffset, clipOffset);
        glEnable(GL_CLIP_DISTANCE0);
    }
    else
        glDisable(GL_CLIP_DISTANCE0);

    if (textureUsed[albedo])
        textures[albedo]->bind(albedo);
    if (textureUsed[normalMap])
        textures[normalMap]->bind(normalMap);
    if (textureUsed[mohr])
        textures[mohr]->bind(mohr);

    glPolygonMode(GL_FRONT_AND_BACK, useWireframe ? GL_LINE : GL_FILL);
}
