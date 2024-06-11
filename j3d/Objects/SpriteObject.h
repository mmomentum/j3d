#pragma once

#include <JuceHeader.h>

#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>

// basically a "sprite" that is a label in 3d
class TextRenderer
{
public:
	TextRenderer(juce::Component& owner);
	~TextRenderer();

	void render(juce::OpenGLShaderProgram* program);

	glm::vec3 position = glm::vec3(0.0f);
	glm::vec2 texture_size = glm::vec2(0.0f);

	GLuint VAO = 0, VBO = 0, EBO = 0;
	juce::OpenGLTexture texture;

	void setText(juce::String);
	void update();

	void setColour(juce::Colour);

private:
	void setup();

	juce::String text;

	bool colour_changed = false;
	juce::Colour colour;

	juce::Component& owner;
};