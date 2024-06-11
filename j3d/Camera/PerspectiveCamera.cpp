#include "PerspectiveCamera.h"

const void PerspectiveCamera::render(juce::OpenGLShaderProgram* currentProgram)
{
	auto camera = getProjectionMatrix();
	auto view = getViewMatrix();

	currentProgram->setUniformMat4("cameraProjection", &camera[0][0], 1, GL_FALSE);
	currentProgram->setUniformMat4("cameraView", &view[0][0], 1, GL_FALSE);

	auto d = quaternionToDirection(rotation);

	currentProgram->setUniform("cameraPosition", position.x, position.y, position.z);
	currentProgram->setUniform("cameraDirection", d.x, d.y, d.z);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////

glm::vec3 PerspectiveCamera::unprojectMouseCoordinate(juce::Rectangle<int> screen_size,
	juce::Point<int> mouse_pos, glm::vec3 plane_origin)
{
	float normalizedX = (2.0f * mouse_pos.x) / screen_size.getWidth() - 1.0f;
	float normalizedY = 1.0f - (2.0f * mouse_pos.y) / screen_size.getHeight();

	// Create the inverse projection matrix
	glm::mat4 inverseProjection = glm::inverse(getProjectionMatrix());

	// Create a 4D vector in homogeneous coordinates
	glm::vec4 rayClip(normalizedX, normalizedY, -1.0f, 1.0f);

	// Transform to eye space
	glm::vec4 rayEye = inverseProjection * rayClip;
	rayEye = glm::vec4(rayEye.x, rayEye.y, -1.0f, 0.0f); // z = -1 indicates the direction in eye space

	auto d = quaternionToDirection(rotation);

	// Transform to world space
	glm::mat4 viewMatrix = glm::lookAt(position, position + d, up);
	glm::mat4 inverseView = glm::inverse(viewMatrix);
	glm::vec4 rayWorld = inverseView * rayEye;

	// Normalize the direction vector
	glm::vec3 rayDir = glm::normalize(glm::vec3(rayWorld));

	float intersectionDistance;
	bool intersects = glm::intersectRayPlane(position, rayDir, plane_origin, -d, intersectionDistance);

	glm::vec3 intersectionPoint;

	if (intersects)
	{
		// Calculate the intersection point using the distance
		intersectionPoint = position + intersectionDistance * rayDir;
	}
	else
	{
		// Set intersection point to NaN if there is no intersection
		intersectionPoint = glm::vec3(std::numeric_limits<float>::quiet_NaN());
		jassertfalse; // something happened...
	}

	return intersectionPoint;
}

glm::vec3 PerspectiveCamera::unprojectMouseCoordinate(juce::Rectangle<int> screen_size, juce::Point<int> mouse_pos, glm::vec3 plane_origin, glm::vec3 plane_normal)
{
	float normalizedX = (2.0f * mouse_pos.x) / screen_size.getWidth() - 1.0f;
	float normalizedY = 1.0f - (2.0f * mouse_pos.y) / screen_size.getHeight();

	// Create the inverse projection matrix
	glm::mat4 inverseProjection = glm::inverse(getProjectionMatrix());

	// Create a 4D vector in homogeneous coordinates
	glm::vec4 rayClip(normalizedX, normalizedY, -1.0f, 1.0f);

	// Transform to eye space
	glm::vec4 rayEye = inverseProjection * rayClip;
	rayEye = glm::vec4(rayEye.x, rayEye.y, -1.0f, 0.0f); // z = -1 indicates the direction in eye space

	auto d = quaternionToDirection(rotation);

	// Transform to world space
	glm::mat4 viewMatrix = glm::lookAt(position, position + d, up);
	glm::mat4 inverseView = glm::inverse(viewMatrix);
	glm::vec4 rayWorld = inverseView * rayEye;

	// Normalize the direction vector
	glm::vec3 rayDir = glm::normalize(glm::vec3(rayWorld));

	float intersectionDistance;
	bool intersects = glm::intersectRayPlane(position, rayDir, plane_origin, plane_normal, intersectionDistance);

	glm::vec3 intersectionPoint;

	if (intersects)
	{
		// Calculate the intersection point using the distance
		intersectionPoint = position + intersectionDistance * rayDir;
	}
	else
	{
		intersectionDistance = farPlane;

		intersectionPoint = position + intersectionDistance * rayDir;
	}

	return intersectionPoint;
}

juce::Point<float> PerspectiveCamera::projectPositionCoordinate(
	juce::Rectangle<int> screen_size, glm::vec3 position)
{
	auto point = projectPositionCoordinateWithZ(screen_size, position);

	return juce::Point<float>(point.x, point.y);
}

glm::vec3 PerspectiveCamera::projectPositionCoordinateWithZ(juce::Rectangle<int> screen_size, glm::vec3 position)
{
	// Combine view and projection matrices
	glm::mat4 viewProjectionMatrix = getProjectionMatrix() * getViewMatrix();

	// Homogeneous coordinates transformation
	glm::vec4 clipCoord = viewProjectionMatrix * glm::vec4(position, 1.0f);

	// Perspective divide
	glm::vec3 normalizedDeviceCoord = glm::vec3(clipCoord) / clipCoord.w;

	// Map to screen space
	glm::vec3 screenCoord;
	screenCoord.x = (normalizedDeviceCoord.x + 1.0f) * 0.5f * float(screen_size.getWidth());
	screenCoord.y = (1.0f - normalizedDeviceCoord.y) * 0.5f * float(screen_size.getHeight());
	screenCoord.z = normalizedDeviceCoord.z;

	return screenCoord;
}