/*
  ==============================================================================

    SceneObject.cpp
    Created: 9 Jul 2023 9:03:07pm
    Author:  Syerjchep

  ==============================================================================
*/

#include "SceneObject.h"

const void meshObject::render(program* currentProgram)
{
    if (toUse)
        toUse->render(currentProgram);

    for (int a = 0; a < meshes.size(); a++)
    {
        glm::mat4 modelMatrix = glm::toMat4(meshRotations[a]);
        modelMatrix = glm::translate(meshPositions[a]) * modelMatrix;
        modelMatrix = glm::toMat4(rotation) * modelMatrix;
        modelMatrix = glm::translate(position) * modelMatrix;

        currentProgram->setModelMatrix(modelMatrix);
        meshes[a]->render();
    }
}

const void lightObject::render(program* currentProgram)
{
    currentProgram->setLightPosition(position);
    currentProgram->setLightColor(color);
}
