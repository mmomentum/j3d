/*
  ==============================================================================

    environment.h
    Created: 25 Aug 2023 12:49:54am
    Author:  Syerjchep

  ==============================================================================
*/

#pragma once

#include "Texture.h"

GLuint processEquirectangularMap(program& rectToCube, std::string fileName, bool mipMaps);
program* rectToCubeProgram();

