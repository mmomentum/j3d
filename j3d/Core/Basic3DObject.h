#pragma once

#include "../Common.h"

#include "Transform3D.h"
#include "Mesh3D.h"
#include "../Material/Material.h"

struct Basic3DObject : public Transform3D
{
	Basic3DObject(juce::String name = "", int idx = -1) : objectName(name) { juce::ignoreUnused(idx); }

	~Basic3DObject();

	void setName(juce::String name) { objectName = name; }
	juce::String getName() { return objectName; }

	virtual const void render(juce::OpenGLShaderProgram* currentProgram,
		glm::mat4 modelMatrix = glm::mat4(1.0), Material3D* inheritedMat = 0);

	// renders to the mouse picking buffer. if pick_children is set to true, then it will attempt
	// to recursively check this object's children if mouse picking is active, and they will be
	// rendered.
	const void renderPicking(juce::OpenGLShaderProgram* currentProgram, bool pick_children = false,
		glm::mat4 modelMatrix = glm::mat4(1.0), Material3D* inheritedMat = 0);

	Basic3DObject* add(Basic3DObject* toAdd);
	Basic3DObject* add(Mesh3D* _theMesh3D, glm::vec3 _position = glm::vec3(0, 0, 0),
		glm::quat _rotation = glm::quat(1, 0, 0, 0), Material3D* _mat = 0);

	void removeChildObject(Basic3DObject* toRemove);

	Basic3DObject* parent = 0;
	Material3D* material = 0;
	Mesh3D* mesh = 0;
	std::vector<Basic3DObject*> children;

	bool visible = true;

	bool outlined = false;

	void setPickingIndex(unsigned int idx) { pick_idx = idx; }

	unsigned int getPickingIndex() { return pick_idx; }

protected:
	unsigned int pick_idx = 0;

private:
	juce::String objectName = "";

	// programatically generates a picking colour to use when doing a mouse pick pass
	// (not used on everything). this 
	glm::ivec3 getPickingColor()
	{
		glm::ivec3 ret;
		ret.x = (pick_idx & 0xFF0000) >> 16;
		ret.y = (pick_idx & 0xFF00) >> 8;
		ret.z = pick_idx & 0xFF;
		return ret;
	}
};
