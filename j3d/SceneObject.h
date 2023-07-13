/*
  ==============================================================================

    SceneObject.h
    Created: 9 Jul 2023 9:03:11pm
    Author:  Syerjchep

  ==============================================================================
*/

#pragma once

#include "Common.h"
#include "Program.h"
#include "Material.h"
#include "Mesh.h"

struct meshObject
{
    glm::vec3 position = glm::vec3(0, 0, 0);
    glm::quat rotation = glm::quat(1, 0, 0, 0);
    Material* theMaterial = 0;
    Mesh* theMesh = 0;
    std::vector<meshObject*> children;

    const void render(program *currentProgram,glm::mat4 modelMatrix = glm::mat4(1.0));
};