#include "Texture.h"

GLenum formatEnum(unsigned int channels, bool flip = false)
{
	if (flip)
	{
		switch (channels)
		{
			case 1: return GL_RED;
			case 2: return GL_RG;
			case 3: return GL_BGR;
			case 4: return GL_BGRA;
		}
	}

	switch (channels)
	{
		case 1: return GL_RED;
		case 2: return GL_RG;
		case 3: return GL_RGB;
		case 4: return GL_RGBA;
	}
}

Texture::Texture(glm::ivec2 dimensions, texturePrefs& desired) : settings(desired)
{
	width = dimensions.x;
	height = dimensions.y;

	glGenTextures(1, &handle);

	//Generate a generic 4 color checkerboard pattern for testing:
	std::vector<unsigned char> pixelData;
	for (int x = 0; x < dimensions.x; x++)
	{
		for (int y = 0; y < dimensions.y; y++)
		{
			int xCell = x / 10.0;
			int yCell = y / 10.0;

			//Red
			if (xCell & 1 && yCell & 1)
			{
				pixelData.push_back(255);
				pixelData.push_back(0);
				pixelData.push_back(0);
				pixelData.push_back(255);
			}
			//Green
			else if (yCell & 1)
			{
				pixelData.push_back(0);
				pixelData.push_back(255);
				pixelData.push_back(0);
				pixelData.push_back(255);
			}
			//Blue
			else if (xCell & 1)
			{
				pixelData.push_back(0);
				pixelData.push_back(0);
				pixelData.push_back(255);
				pixelData.push_back(255);
			}
			//White
			else
			{
				pixelData.push_back(255);
				pixelData.push_back(255);
				pixelData.push_back(255);
				pixelData.push_back(255);
			}
		}
	}

	glBindTexture(GL_TEXTURE_2D, handle);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, settings.wrapS);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, settings.wrapT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, settings.magFilter);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, settings.minFilter);

	glTexImage2D(GL_TEXTURE_2D, 0, formatEnum(settings.channels), width, height, 0, formatEnum(settings.channels), GL_UNSIGNED_BYTE, &pixelData[0]);

	if (settings.minFilter == GL_LINEAR_MIPMAP_LINEAR ||
		settings.minFilter == GL_NEAREST_MIPMAP_NEAREST ||
		settings.minFilter == GL_LINEAR_MIPMAP_NEAREST ||
		settings.minFilter == GL_NEAREST_MIPMAP_LINEAR)
		glGenerateMipmap(GL_TEXTURE_2D);

	glBindTexture(GL_TEXTURE_2D, 0);

	valid = true;
}

Texture::Texture(const char* filename, texturePrefs& desired) : settings(desired)
{
	settings = desired;
	glGenTextures(1, &handle);

	juce::Image source = juce::ImageFileFormat::loadFrom(juce::File(filename));

	if (!source.isValid())
	{
		DBG("Could not load texture " + std::string(filename));
		return;
	}

	if (source.isSingleChannel())
		settings.channels = 1;
	else if (source.isRGB())
		settings.channels = 3;
	else if (source.isARGB())
		settings.channels = 4;
	else
	{
		DBG("Invalid format / channels from loaded image!");
		return;
	}

	width = source.getWidth();
	height = source.getHeight();

	glBindTexture(GL_TEXTURE_2D, handle);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, settings.wrapS);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, settings.wrapT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, settings.magFilter);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, settings.minFilter);

	auto bitmapData = juce::Image::BitmapData(source, juce::Image::BitmapData::ReadWriteMode::readOnly);
	glTexImage2D(GL_TEXTURE_2D, 0, formatEnum(settings.channels), width, height, 0, formatEnum(settings.channels, true), GL_UNSIGNED_BYTE, bitmapData.data);

	if (settings.minFilter == GL_LINEAR_MIPMAP_LINEAR ||
		settings.minFilter == GL_NEAREST_MIPMAP_NEAREST ||
		settings.minFilter == GL_LINEAR_MIPMAP_NEAREST ||
		settings.minFilter == GL_NEAREST_MIPMAP_LINEAR)
		glGenerateMipmap(GL_TEXTURE_2D);

	glBindTexture(GL_TEXTURE_2D, 0);

	valid = true;
}

Texture::Texture(const char* binary_data, const int binary_data_size, texturePrefs& desired) : settings(desired)
{
	settings = desired;
	glGenTextures(1, &handle);

	juce::Image image = juce::ImageCache::getFromMemory(binary_data, binary_data_size);

	if (!image.isValid())
	{
		DBG("Binary Data does not create a valid image.");
		return;
	}

	if (image.isSingleChannel())
		settings.channels = 1;
	else if (image.isRGB())
		settings.channels = 3;
	else if (image.isARGB())
		settings.channels = 4;
	else
	{
		DBG("Invalid format / channels from loaded image!");
		return;
	}

	width = image.getWidth();
	height = image.getHeight();

	glBindTexture(GL_TEXTURE_2D, handle);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, settings.wrapS);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, settings.wrapT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, settings.magFilter);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, settings.minFilter);

	auto bitmapData = juce::Image::BitmapData(image, juce::Image::BitmapData::ReadWriteMode::readOnly);
	glTexImage2D(GL_TEXTURE_2D, 0, formatEnum(settings.channels), width, height, 0, formatEnum(settings.channels, true), GL_UNSIGNED_BYTE, bitmapData.data);

	if (settings.minFilter == GL_LINEAR_MIPMAP_LINEAR ||
		settings.minFilter == GL_NEAREST_MIPMAP_NEAREST ||
		settings.minFilter == GL_LINEAR_MIPMAP_NEAREST ||
		settings.minFilter == GL_NEAREST_MIPMAP_LINEAR)
		glGenerateMipmap(GL_TEXTURE_2D);

	glBindTexture(GL_TEXTURE_2D, 0);

	valid = true;
}

Texture::Texture(juce::Image& source, texturePrefs& desired) : settings(desired)
{
	settings = desired;
	glGenTextures(1, &handle);

	// source should not be 0 width or height!
	jassert(source.getWidth() != 0 || source.getHeight() != 0);

	if (source.isSingleChannel())
		settings.channels = 1;
	else if (source.isRGB())
		settings.channels = 3;
	else if (source.isARGB())
		settings.channels = 4;
	else
	{
		DBG("Invalid format / channels from loaded image!");
		return;
	}

	width = source.getWidth();
	height = source.getHeight();

	glBindTexture(GL_TEXTURE_2D, handle);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, settings.wrapS);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, settings.wrapT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, settings.magFilter);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, settings.minFilter);

	auto bitmapData = juce::Image::BitmapData(source, juce::Image::BitmapData::ReadWriteMode::readOnly);
	glTexImage2D(GL_TEXTURE_2D, 0, formatEnum(settings.channels), width, height, 0, formatEnum(settings.channels, true), GL_UNSIGNED_BYTE, bitmapData.data);

	if (settings.minFilter == GL_LINEAR_MIPMAP_LINEAR ||
		settings.minFilter == GL_NEAREST_MIPMAP_NEAREST ||
		settings.minFilter == GL_LINEAR_MIPMAP_NEAREST ||
		settings.minFilter == GL_NEAREST_MIPMAP_LINEAR)
		glGenerateMipmap(GL_TEXTURE_2D);

	glBindTexture(GL_TEXTURE_2D, 0);

	valid = true;
}

Texture::~Texture()
{
	glDeleteTextures(1, &handle);
}

const void Texture::bind(textureBindNames bindLocation)
{
	glActiveTexture(GL_TEXTURE0 + bindLocation);
	glBindTexture(GL_TEXTURE_2D, handle);
}
