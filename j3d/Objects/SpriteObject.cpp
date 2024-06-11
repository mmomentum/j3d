#include "TextRenderer.h"

namespace
{
	void renderTextTexture(const juce::String& text, juce::Font& font, juce::OpenGLTexture& texture,
		glm::vec2& out_size, juce::Colour colour)
	{
		auto image = juce::Image(juce::Image::PixelFormat::ARGB,
			font.getStringWidth(text), font.getHeight(), true);

		auto g = juce::Graphics(image);

		g.setColour(colour);
		g.setFont(font);

		g.drawText(text,
			0, 0, image.getWidth(), image.getHeight(),
			juce::Justification::centred, true);

		texture.loadImage(image);

		out_size = glm::vec2(image.getWidth(), image.getHeight());
	}
}

TextRenderer::TextRenderer(juce::Component& o) : owner(o)
{
	colour = owner.findColour(juce::Label::textColourId);

	setup();
	setText("Label");
}

TextRenderer::~TextRenderer()
{

}

void TextRenderer::render(juce::OpenGLShaderProgram* program)
{
	if (colour_changed)
		setText(text);

	using namespace juce::gl;

	program->setUniform("spritePosition", position.x, position.y, position.z);
	program->setUniform("spriteSize", texture_size.x, texture_size.y);

	program->setUniform("screenSize", owner.getWidth(), owner.getHeight());

	glBindVertexArray(VAO);
	glBindTexture(GL_TEXTURE_2D, texture.getTextureID());
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}

void TextRenderer::setText(juce::String string)
{
	text = string;

	auto f = owner.getLookAndFeel().getAlertWindowMessageFont();

	f.setHeight(f.getHeight() * 1.2f);

	renderTextTexture(string, f, texture, texture_size, colour);

	colour_changed = false;
}

void TextRenderer::setColour(juce::Colour c)
{
	colour = c;

	colour_changed = true;
}

void TextRenderer::setup()
{
	using namespace juce::gl;

	float vertices[] = {
		// positions       // texture coords
		0.5f,  0.5f, 0.0f, 1.0f, 1.0f,
		0.5f, -0.5f, 0.0f, 1.0f, 0.0f,
	   -0.5f, -0.5f, 0.0f, 0.0f, 0.0f,
	   -0.5f,  0.5f, 0.0f, 0.0f, 1.0f
	};

	unsigned int indices[] = {
		0, 1, 3,
		1, 2, 3
	};

	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	// Position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	// Texture coord attribute
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}
