/*
  ==============================================================================

    Scene.h
    Created: 8 Jul 2023 1:33:16am
    Author:  Syerjchep

  ==============================================================================
*/

#pragma once

#include "SceneObject.h"
#include "Camera.h"

struct scene
{
    program* toUse = 0;
    sceneObject* camera = 0;
    glm::vec3 lightColor = { 1,1,1 };
    glm::vec3 lightPosition = { 10,7,5 };
    std::vector<meshObject*> scene;

    const void render();
};