/*
  ==============================================================================

    SceneObject.cpp
    Created: 9 Jul 2023 9:03:07pm
    Author:  Syerjchep

  ==============================================================================
*/

#include "SceneObject.h"

unsigned int meshInstance::lastMeshInstanceID = 0;

const void meshInstance::render(program* currentProgram,glm::mat4 modelMatrix, Material* inheritedMat)
{
    if (theMaterial)
        theMaterial->render(currentProgram);
    else if (inheritedMat)
        inheritedMat->render(currentProgram);

    modelMatrix = glm::scale(scale) * modelMatrix;
    modelMatrix = glm::toMat4(rotation) * modelMatrix;
    modelMatrix = glm::translate(position) * modelMatrix;
    currentProgram->setModelMatrix(modelMatrix);

    if (theMesh)
        theMesh->render();

    for (int a = 0; a < children.size(); a++)
        children[a]->render(currentProgram, modelMatrix, theMaterial ? theMaterial : inheritedMat);
}

const void meshInstance::renderPicking(program* currentProgram, glm::mat4 modelMatrix)
{
    glm::ivec3 pc = getPickingColor();
    glUniform3i(currentProgram->getUniformLocation("pickingColor"),  pc.x, pc.y, pc.z);

    modelMatrix = glm::scale(scale) * modelMatrix;
    modelMatrix = glm::toMat4(rotation) * modelMatrix;
    modelMatrix = glm::translate(position) * modelMatrix;
    currentProgram->setModelMatrix(modelMatrix);

    if (theMesh)
        theMesh->render();

    for (int a = 0; a < children.size(); a++)
        children[a]->renderPicking(currentProgram, modelMatrix);
}

meshInstance::meshInstance()
{
    meshInstances.push_back(this);

    instanceID = lastMeshInstanceID;
    lastMeshInstanceID++;
}

meshInstance::~meshInstance()
{
    for (int a = 0; a < meshInstances.size(); a++)
    {
        if (meshInstances[a] == this)
        {
            meshInstances.erase(meshInstances.begin() + a);
            break;
        }
    }

    for (int a = 0; a < children.size(); a++)
    {
        if (children[a])
        {
            delete children[a];
            children[a] = 0;
        }
    }
}

meshInstance* meshInstance::add(meshInstance* toAdd)
{
    children.push_back(toAdd);
    toAdd->parent = this;
    return toAdd;
}

meshInstance* meshInstance::add(Mesh* _theMesh, glm::vec3 _position, glm::quat _rotation , Material* _mat )
{
    meshInstance* tmp = new meshInstance();
    tmp->theMesh = _theMesh;
    tmp->position = _position;
    tmp->rotation = _rotation;
    tmp->theMaterial = _mat;

    return add(tmp);
}