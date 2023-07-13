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

struct meshObject : sceneObject
{
    Material* toUse = 0;
    std::vector<Mesh*> meshes;
    std::vector<glm::vec3> meshPositions;
    std::vector<glm::quat> meshRotations;

    const void render(program *currentProgram);
};

struct lightObject : sceneObject
{
    glm::vec3 color = glm::vec3(1.0, 0.75, 0.25);
    
    bool coneLight = false;
    float coneRadians = 3.1415;

    const void render(program* currentProgram);
};