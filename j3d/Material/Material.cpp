#include "Material.h"

void Material3D::setClipping(glm::vec3 axis, float offset)
{
	doClipping = true;
	clipPlaneDirection = glm::normalize(axis);
	clipOffset = offset;
}

void Material3D::setBaseColour(juce::Colour colour)
{
	colour_rgba.x = colour.getFloatRed();
	colour_rgba.y = colour.getFloatGreen();
	colour_rgba.z = colour.getFloatBlue();
}

void Material3D::setAlpha(float alpha0to1)
{
	alpha_tmp = alpha0to1;

	colour_rgba.w = alpha_tmp;
}

juce::Colour Material3D::getBaseColour()
{
	return juce::Colour::fromFloatRGBA(colour_rgba.x, colour_rgba.y, colour_rgba.z, 1.0f);
}

void Material3D::setOutlineColour(juce::Colour colour)
{
	colour_outline.x = colour.getFloatRed();
	colour_outline.y = colour.getGreen();
	colour_outline.z = colour.getFloatBlue();
}

const void Material3D::render(juce::OpenGLShaderProgram* boundProgram)
{
	if (doClipping)
	{
		boundProgram->setUniform("clipPlaneDirection", clipPlaneDirection.x,
			clipPlaneDirection.y, clipPlaneDirection.z);
		boundProgram->setUniform("clipPlaneOffset", clipOffset);

		glEnable(GL_CLIP_DISTANCE0);
	}
	else
		glDisable(GL_CLIP_DISTANCE0);

	boundProgram->setUniform("baseColor", colour_rgba.x, colour_rgba.y,
		colour_rgba.z, colour_rgba.w);

	boundProgram->setUniform("outlineColor", colour_outline.x, colour_outline.y,
		colour_outline.z, colour_outline.w);

	boundProgram->setUniform("useMatcap", matcap_used);

	glPolygonMode(GL_FRONT_AND_BACK, useWireframe ? GL_LINE : GL_FILL);
}
