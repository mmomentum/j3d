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

struct meshInstance
{
    glm::vec3 position = glm::vec3(0, 0, 0);
    glm::quat rotation = glm::quat(1, 0, 0, 0);
    glm::vec3 scale = glm::vec3(1, 1, 1);

    meshInstance* parent = 0;
    Material* theMaterial = 0;
    Mesh* theMesh = 0;
    std::vector<meshInstance*> children;

    const void render(program *currentProgram,glm::mat4 modelMatrix = glm::mat4(1.0),Material *inheritedMat = 0);

    meshInstance* add(meshInstance* toAdd);
    meshInstance* add(Mesh* _theMesh, glm::vec3 _position = glm::vec3(0, 0, 0), glm::quat _rotation = glm::quat(1, 0, 0, 0), Material* _mat = 0);

    ~meshInstance();

    //TODO: Add copy and move operators
};
