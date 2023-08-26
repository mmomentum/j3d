#pragma once

#include "Texture.h"

GLuint processEquirectangularMap(program& rectToCube, Texture& fileName, bool mipMaps);
program* rectToCubeProgram();
GLuint createBDRFLUTTexture();

