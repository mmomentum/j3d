#pragma once

#include "Common.h"
#include "Program.h"

//This file can only be included once in the whole project, and thus cannot go in common.h:
/*#define STB_IMAGE_IMPLEMENTATION
#include "../depends/stb_image.h"*/

class Texture
{
private:
	GLuint handle = 0;
	unsigned int width = 0;
	unsigned int height = 0;
	bool valid = false;

	struct texturePrefs
	{
		unsigned int channels = 4;
		GLenum wrapS = GL_REPEAT, wrapT = GL_REPEAT;
		GLenum minFilter = GL_LINEAR_MIPMAP_LINEAR;
		GLenum magFilter = GL_LINEAR;
	} settings;

public:
	// Generates a basic checkerboard test texture
	Texture(glm::ivec2 dimensions, texturePrefs& desired = texturePrefs());

	// Loads a texture from a juce::Image
	Texture(juce::Image image, texturePrefs& desired = texturePrefs());

	// Loads a texture from a filepath
	Texture(const char* filename, texturePrefs& desired = texturePrefs());

	// Loads a texture from Binary Data
	Texture(const char* binary_data, const int binary_data_size,
		texturePrefs& desired = texturePrefs());

	~Texture();

	const inline unsigned int getWidth() { return width; }
	const inline unsigned int getHeight() { return height; }
	const inline bool isValid() { return valid; }

	const void bind(textureBindNames bindLocation);
};
