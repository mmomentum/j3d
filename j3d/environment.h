#pragma once

#include "Texture.h"

GLuint processEquirectangularMap(program& rectToCube, std::string filePath, bool mipMaps);
GLuint processEquirectangularMap(program& rectToCube, Texture& fileName, bool mipMaps);
program* rectToCubeProgram();
GLuint createBDRFLUTTexture();

