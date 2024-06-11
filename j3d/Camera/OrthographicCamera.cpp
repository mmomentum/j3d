#include "OrthographicCamera.h"

const void OrthographicCamera::render(juce::OpenGLShaderProgram* currentProgram)
{
	auto camera = getProjectionMatrix();
	auto view = getViewMatrix();

	currentProgram->setUniformMat4("cameraProjection", &camera[0][0], 1, GL_FALSE);
	currentProgram->setUniformMat4("cameraView", &view[0][0], 1, GL_FALSE);

	auto d = quaternionToDirection(rotation);

	currentProgram->setUniform("cameraPosition", position.x, position.y, position.z);
	currentProgram->setUniform("cameraDirection", d.x, d.y, d.z);
}

glm::vec3 OrthographicCamera::unprojectMouseCoordinate(juce::Rectangle<int> screen_size,
	juce::Point<int> mouse_pos, glm::vec3 plane_origin)
{
	auto d = quaternionToDirection(rotation);

	glm::vec3 camera_direction = d;
	glm::vec3 camera_right = glm::normalize(glm::cross(camera_direction, up));
	glm::vec3 camera_up = glm::normalize(glm::cross(camera_right, camera_direction));

	float ortho_tmp = ortho_scale * zoom_value;

	// Calculate left and right ortho size based on the aspect ratio
	float ortho_x = ortho_tmp * (float(screen_size.getWidth()) / float(screen_size.getHeight()));

	// Calculate relative mouse coordinates
	float x_relative = +(2.0f * mouse_pos.x / screen_size.getWidth() - 1) * ortho_x;
	float y_relative = -(2.0f * mouse_pos.y / screen_size.getHeight() - 1) * ortho_tmp;

	// Calculate ray origin
	glm::vec3 ray_origin = position + (camera_right * x_relative) + (camera_up * y_relative);
	glm::vec3 ray_direction = camera_direction;

	// Calculate plane / volume intersection
	float intersect_distance = 0.0f;
	bool intersect = glm::intersectRayPlane(ray_origin, ray_direction, plane_origin, -d, intersect_distance);

	// if intersect fails, then we intersect with the ray direction as the negative (so it's casting behind
	// the actual location of the camera. it sounds weird but with the nearplane being the way it is there
	// is no other way to do it)
	if (!intersect)
	{
		bool back_intersect = glm::intersectRayPlane(ray_origin,
			-ray_direction, plane_origin, -d, intersect_distance);

		// if this fails, then something *really* must be wrong
		jassert(back_intersect);

		return ray_origin + intersect_distance * -ray_direction;
	}

	// Calculate intersection point
	glm::vec3 plane_intersect = ray_origin + intersect_distance * ray_direction;

	return plane_intersect;
}

glm::vec3 OrthographicCamera::unprojectMouseCoordinate(juce::Rectangle<int> screen_size,
	juce::Point<int> mouse_pos, glm::vec3 plane_origin, glm::vec3 plane_normal)
{
	auto d = quaternionToDirection(rotation);

	glm::vec3 camera_direction = d;
	glm::vec3 camera_right = glm::normalize(glm::cross(camera_direction, up));
	glm::vec3 camera_up = glm::normalize(glm::cross(camera_right, camera_direction));

	float ortho_tmp = ortho_scale * zoom_value;

	// Calculate left and right ortho size based on the aspect ratio
	float ortho_x = ortho_tmp * (float(screen_size.getWidth()) / float(screen_size.getHeight()));

	// Calculate relative mouse coordinates
	float x_relative = +(2.0f * mouse_pos.x / screen_size.getWidth() - 1) * ortho_x;
	float y_relative = -(2.0f * mouse_pos.y / screen_size.getHeight() - 1) * ortho_tmp;

	// Calculate ray origin
	glm::vec3 ray_origin = position + (camera_right * x_relative) + (camera_up * y_relative);
	glm::vec3 ray_direction = camera_direction;

	// Calculate plane / volume intersection
	float intersect_distance = 0.0f;
	bool intersect = glm::intersectRayPlane(ray_origin, ray_direction, plane_origin, plane_normal, intersect_distance);

	if (!intersect)
	{
		bool back_intersect = glm::intersectRayPlane(ray_origin,
			-ray_direction, plane_origin, plane_normal, intersect_distance);

		jassert(back_intersect);

		return ray_origin + intersect_distance * -ray_direction;
	}

	// Calculate intersection point
	glm::vec3 plane_intersect = ray_origin + intersect_distance * ray_direction;

	return plane_intersect;
}

juce::Point<float> OrthographicCamera::projectPositionCoordinate(juce::Rectangle<int> screen_size, glm::vec3 position)
{
	auto point = projectPositionCoordinateWithZ(screen_size, position);

	return juce::Point<float>(point.x, point.y);
}

glm::vec3 OrthographicCamera::projectPositionCoordinateWithZ(juce::Rectangle<int> screen_size, glm::vec3 position)
{
	// Combine view and projection matrices
	glm::mat4 viewProjectionMatrix = getProjectionMatrix() * getViewMatrix();

	// Apply translation to the position vector
	glm::vec4 positionWithTranslation = glm::vec4(position, 1.0f);

	// Homogeneous coordinates transformation
	glm::vec4 clipCoord = viewProjectionMatrix * positionWithTranslation;

	// Map to screen space
	glm::vec3 screenCoord;
	screenCoord.x = (clipCoord.x + 1.0f) * 0.5f * float(screen_size.getWidth());
	screenCoord.y = (1.0f - clipCoord.y) * 0.5f * float(screen_size.getHeight());
	screenCoord.z = clipCoord.z;

	return screenCoord;
}
