/*
  ==============================================================================

    SceneObject.cpp
    Created: 9 Jul 2023 9:03:07pm
    Author:  Syerjchep

  ==============================================================================
*/

#include "SceneObject.h"

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

meshInstance::~meshInstance()
{
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