#pragma once

#include "Common.h"
#include "Program.h"

class Texture
{
private:
	GLuint handle = 0;
	unsigned int width = 0;
	unsigned int height = 0;
	bool valid = false;

    // clang / gcc really does not like it when this struct has the member variables
    // initialized, so they are defined in brackets outside instead
	struct texturePrefs
	{
		texturePrefs()
		{
			channels = 4;
			wrapS = GL_REPEAT;
			wrapT = GL_REPEAT;

			minFilter = GL_LINEAR_MIPMAP_LINEAR;
			magFilter = GL_LINEAR;
		};

		unsigned int channels;

		GLenum wrapS;
		GLenum wrapT;

		GLenum minFilter;
		GLenum magFilter;
	} settings;

public:
	// Generates a basic checkerboard test texture
	Texture(glm::ivec2 dimensions, texturePrefs desired = texturePrefs());

	// Loads a texture from a juce::Image
	Texture(juce::Image& image, texturePrefs desired = texturePrefs());

	// Loads a texture from a filepath
	Texture(const char* filename, texturePrefs desired = texturePrefs());

	// Loads a texture from Binary Data
	Texture(const char* binary_data, const int binary_data_size,
		texturePrefs desired = texturePrefs());

	~Texture();

	const inline unsigned int getWidth() { return width; }
	const inline unsigned int getHeight() { return height; }
	const inline bool isValid() { return valid; }

	const void bind(textureBindNames bindLocation);
};
