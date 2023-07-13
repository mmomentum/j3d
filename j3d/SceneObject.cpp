/*
  ==============================================================================

    SceneObject.cpp
    Created: 9 Jul 2023 9:03:07pm
    Author:  Syerjchep

  ==============================================================================
*/

#include "SceneObject.h"

const void meshObject::render(program* currentProgram,glm::mat4 modelMatrix)
{
    if (theMaterial)
        theMaterial->render(currentProgram);
    modelMatrix = glm::toMat4(rotation) * modelMatrix;
    modelMatrix = glm::translate(position) * modelMatrix;
    currentProgram->setModelMatrix(modelMatrix);
    if (theMesh)
        theMesh->render();

    for (int a = 0; a < children.size(); a++)
        children[a]->render(currentProgram, modelMatrix);
}
