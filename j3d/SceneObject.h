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
    static unsigned int lastMeshInstanceID;
    static std::vector<meshInstance*> meshInstances;

    glm::vec3 position = glm::vec3(0, 0, 0);
    glm::quat rotation = glm::quat(1, 0, 0, 0);
    glm::vec3 scale = glm::vec3(1, 1, 1);

    bool outlined = false;

    unsigned int instanceID;

    meshInstance* parent = 0;
    Material* theMaterial = 0;
    Mesh* theMesh = 0;
    std::vector<meshInstance*> children;

    const void render(program *currentProgram,glm::mat4 modelMatrix = glm::mat4(1.0),Material *inheritedMat = 0);
    const void renderPicking(program *currentProgram,glm::mat4 modelMatrix = glm::mat4(1.0));

    meshInstance* add(meshInstance* toAdd);
    meshInstance* add(Mesh* _theMesh, glm::vec3 _position = glm::vec3(0, 0, 0), glm::quat _rotation = glm::quat(1, 0, 0, 0), Material* _mat = 0);

    ~meshInstance();
    meshInstance();

    glm::ivec3 getPickingColor()
    {
        glm::ivec3 ret;
        ret.x = (instanceID & 0xFF0000) >> 16;
        ret.y = (instanceID & 0xFF00) >> 8;
        ret.z = instanceID & 0xFF;
        return ret;
    }

    //TODO: Add copy and move operators
};
