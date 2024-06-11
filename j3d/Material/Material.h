#pragma once

#include "../Common.h"

class Material3D
{
public:
	~Material3D() { DBG("Destructing Material"); }

	void setClipping(glm::vec3 axis, float offset);
	inline void disableClipping() { doClipping = false; }

	void setUsingMatcap(bool state) { matcap_used = true; }

	void setBaseColour(juce::Colour);
	void setAlpha(float alpha0to1);

	juce::Colour getBaseColour();
	float getAlpha() { return alpha_tmp; }

	void setOutlineColour(juce::Colour);

	inline void wireframeMode(bool value) { useWireframe = value; }
	const void render(juce::OpenGLShaderProgram* boundProgram);

	void setMatcap(bool state) { matcap_used = state; }

private:
	bool useWireframe = false;

	bool matcap_used = false;

	glm::vec3 clipPlaneDirection = glm::vec3(1.0, 1.0, 0);
	float clipOffset = 0.5;
	bool doClipping = false;

	glm::vec4 colour_rgba{ 1.0f, 0.0f, 0.0f, 0.5f };
	float alpha_tmp = 0.5f;

	glm::vec4 colour_outline{ 1.0f, 1.0f, 1.0f, 1.0f };
};