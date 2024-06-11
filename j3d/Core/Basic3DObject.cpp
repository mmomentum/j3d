#include "Basic3DObject.h"

Basic3DObject::~Basic3DObject()
{
	DBG("Destructing Basic3DObject " + getName());
}

const void Basic3DObject::render(juce::OpenGLShaderProgram* currentProgram,
	glm::mat4 parentModelMatrix, Material3D* inheritedMat)
{
	// dont do anything if we're invisible
	if (!visible)
		return;

	if (material)
		material->render(currentProgram);
	else if (inheritedMat)
		inheritedMat->render(currentProgram);

	glm::mat4 localModelMatrix = glm::translate(position) * glm::toMat4(rotation) * glm::scale(scale);

	// Combine parent and local transformations
	glm::mat4 modelMatrix = parentModelMatrix * localModelMatrix;

	currentProgram->setUniformMat4("modelMatrix", &modelMatrix[0][0], 1, GL_FALSE);

	if (mesh)
	{
		if (outlined)
		{
			// we remove depth masking and then run the shader with the drawOutline boolean
			// to do an outline prior to rendering the actual mesh (with depth mask on)

			glDepthMask(GL_FALSE);
			currentProgram->setUniform("drawOutline", true);
			mesh->render();
			currentProgram->setUniform("drawOutline", false);
			glDepthMask(GL_TRUE);
		}

		mesh->render();
	}

	for (int a = 0; a < children.size(); a++)
		children[a]->render(currentProgram, modelMatrix, material ? material : inheritedMat);
}

const void Basic3DObject::renderPicking(juce::OpenGLShaderProgram* currentProgram,
	bool pick_children, glm::mat4 parentModelMatrix, Material3D* inheritedMat)
{
	glm::mat4 localModelMatrix = glm::translate(position) * glm::toMat4(rotation) * glm::scale(scale);

	// Combine parent and local transformations
	glm::mat4 modelMatrix = parentModelMatrix * localModelMatrix;

	if (pick_idx != 0 && visible)
	{
		if (material)
			material->render(currentProgram);
		else if (inheritedMat)
			inheritedMat->render(currentProgram);

		glm::ivec3 pick_colour = getPickingColor();

		currentProgram->setUniformMat4("modelMatrix", &modelMatrix[0][0], 1, GL_FALSE);

		// ensure we aren't doing anything weird with having the outline be part of the pick mesh
		currentProgram->setUniform("drawOutline", false);

		currentProgram->setUniform("pickingColor",
			pick_colour.x, pick_colour.y, pick_colour.z);

		if (mesh)
			mesh->render();
	}

	// we can still render children of the mesh even if the owner's pick index is invalid
	if (pick_children && visible)
		for (int a = 0; a < children.size(); a++)
			children[a]->renderPicking(currentProgram, pick_children,
				modelMatrix, material ? material : inheritedMat);
}

Basic3DObject* Basic3DObject::add(Basic3DObject* toAdd)
{
	children.push_back(toAdd);
	toAdd->parent = this;
	return toAdd;
}

Basic3DObject* Basic3DObject::add(Mesh3D* _mesh, glm::vec3 _position,
	glm::quat _rotation, Material3D* _mat)
{
	Basic3DObject* tmp = new Basic3DObject();
	tmp->mesh = _mesh;
	tmp->position = _position;
	tmp->rotation = _rotation;
	tmp->material = _mat;

	return add(tmp);
}

void Basic3DObject::removeChildObject(Basic3DObject* toRemove)
{
	children.erase(
		std::remove(children.begin(), children.end(), toRemove), children.end());
	toRemove->parent = nullptr;
}