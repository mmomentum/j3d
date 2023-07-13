/*
  ==============================================================================

    Scene.cpp
    Created: 8 Jul 2023 1:33:12am
    Author:  Syerjchep

  ==============================================================================
*/

#include "Scene.h"

const void scene::render()
{
    camera->render(toUse);
    toUse->use();
    toUse->setLightColor(lightColor);
    toUse->setLightPosition(lightPosition);
    for (int a = 0; a < scene.size(); a++)
        scene[a]->render(toUse);
}