#include "CameraOrbitControls.h"

CameraOrbitControls::CameraOrbitControls(Camera* cam, juce::Component& comp) :
	CameraControlsBase(cam, comp)
{
	initializeValues();
	updatePosition();
}

CameraOrbitControls::~CameraOrbitControls()
{
}

void CameraOrbitControls::setNewCamera(Camera* c)
{
	camera = c;

	updatePosition();
}

juce::String CameraOrbitControls::serializeControlState()
{
	std::ostringstream oss;

	oss << azimuth << ' ' << elevation << ' ' << camera->zoom_value << ' '
		<< origin_point.x << ' ' << origin_point.y << ' ' << origin_point.z << ' '
		<< lastCameraPosition.x << ' ' << lastCameraPosition.y << ' ' << lastCameraPosition.z << ' '
		<< lastMousePosition.x << ' ' << lastMousePosition.y;

	return oss.str();
}

void CameraOrbitControls::deserializeControlState(juce::String string)
{
	std::istringstream iss(string.toStdString());

	iss >> azimuth >> elevation >> camera->zoom_value;
	iss >> origin_point.x >> origin_point.y >> origin_point.z;
	iss >> lastCameraPosition.x >> lastCameraPosition.y >> lastCameraPosition.z;
	iss >> lastMousePosition.x >> lastMousePosition.y;

	updatePosition();
}

void CameraOrbitControls::addAzimuth(float a)
{
	azimuth += a;

	updatePosition();
}

void CameraOrbitControls::addElevation(float e)
{
	elevation += e;

	updatePosition();
}

void CameraOrbitControls::updatePosition()
{
	float elev_radian = glm::radians(elevation);
	float azim_radian = glm::radians(azimuth);

	float x = std::sin(elev_radian) * std::cos(azim_radian);
	float y = std::cos(elev_radian);
	float z = std::sin(elev_radian) * std::sin(azim_radian);

	if (fabs(glm::dot(Camera::quaternionToDirection(camera->rotation), glm::vec3(0, 1, 0))) == 1)
		elevation += 0.01;

	if (elevation < 0)
		elevation += 360;
	if (elevation > 360)
		elevation -= 360;

	if (elevation > 180)
		camera->up = glm::vec3(0, 1, 0); // facing up
	else
		camera->up = glm::vec3(0, -1, 0); // facing down

	camera->position = glm::vec3(x, y, z) * camera->zoom_value + origin_point;
	camera->rotation = Camera::directionToQuaternion(-glm::vec3(x, y, z));
	camera->zoom_value = camera->zoom_value;
}

void CameraOrbitControls::mouseDown(const juce::MouseEvent& e)
{
	lastMousePosition = e.getPosition();
	lastCameraPosition = origin_point;
}

void CameraOrbitControls::mouseDrag(const juce::MouseEvent& e)
{
	// pan
	if (e.mods.isMiddleButtonDown() && e.mods.isShiftDown())
	{
		float x = -e.getDistanceFromDragStartX() * (camera->zoom_value / 150.0f);
		float y = e.getDistanceFromDragStartY() * (camera->zoom_value / 150.0f);

		float elev_radian = glm::radians(elevation);
		float azim_radian = glm::radians(azimuth);

		glm::vec3 viewDir = glm::vec3(
			std::sin(elev_radian) * std::cos(azim_radian),
			std::cos(elev_radian),
			std::sin(elev_radian) * std::sin(azim_radian)
		);

		glm::vec3 rightDir = glm::normalize(glm::cross(viewDir, camera->up));
		glm::vec3 upDir = glm::normalize(glm::cross(rightDir, viewDir));

		// Adjust the camera's origin based on the mouse offset
		origin_point = lastCameraPosition - x * rightDir + y * upDir;

		updatePosition();

		return;
	}

	// orbit
	if (e.mods.isMiddleButtonDown())
	{
		const float scale = 0.5f;

		azimuth = azimuth + (scale *
			(e.getPosition().getX() - lastMousePosition.getX()));

		elevation = elevation + (scale *
			(e.getPosition().getY() - lastMousePosition.getY()));

		lastMousePosition = e.getPosition();

		updatePosition();

		return;
	}

}

void CameraOrbitControls::mouseWheelMove(const juce::MouseEvent& e, const juce::MouseWheelDetails& d)
{
	camera->zoom_value -= d.deltaY;

	camera->zoom_value = juce::jlimit(0.1f, 100.0f, camera->zoom_value);

	updatePosition();
}

bool CameraOrbitControls::keyPressed(const juce::KeyPress& key, juce::Component* originatingComponent)
{
	bool return_state = false;

	return return_state;
}

void CameraOrbitControls::initializeValues()
{
	// take the values from the camera reference and determine the current azimuth, 
	// elevation and origin point (we use the zoom distance value that already exists to
	// figure out the origin point)

	glm::vec3 normalizedDir = glm::normalize(Camera::quaternionToDirection(camera->rotation));

	// Calculate the origin point based on the zoom distance
	origin_point = camera->position + normalizedDir * camera->zoom_value;

	// Calculate the direction vector from the camera position to the origin
	glm::vec3 dirToOrigin = glm::normalize(origin_point - camera->position);

	// Calculate azimuth (angle in the XZ plane)
	azimuth = glm::degrees(atan2(dirToOrigin.z, dirToOrigin.x));

	// Calculate elevation (angle from the XZ plane)
	elevation = glm::degrees(asin(dirToOrigin.y));
}
