/*
  ==============================================================================

    Common.h
    Created: 8 Jul 2023 1:35:19am
    Author:  Syerjchep

  ==============================================================================
*/

#pragma once

#include <fstream>
#include <JuceHeader.h>
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>

using namespace juce::gl;

struct object3D
{
    glm::vec3 position = glm::vec3(0, 0, 0);
    glm::quat rotation = glm::quat(1, 0, 0, 0);
};
