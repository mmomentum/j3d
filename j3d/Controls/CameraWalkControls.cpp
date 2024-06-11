#include "CameraWalkControls.h"

CameraWalkControls::CameraWalkControls(Camera* c, juce::Component& v) :
	CameraControlsBase(c, v)
{
	auto d = Camera::quaternionToDirection(camera->rotation);

	azimuth = glm::degrees(std::atan2(d.z, d.x));
	elevation = glm::degrees(std::asin(d.y));

	key_presses.add(juce::KeyPress(juce::KeyPress::upKey));
	key_presses.add(juce::KeyPress(juce::KeyPress::leftKey));
	key_presses.add(juce::KeyPress(juce::KeyPress::downKey));
	key_presses.add(juce::KeyPress(juce::KeyPress::rightKey));

	// key characters
	for (char c : "wasd")
		key_presses.add(juce::KeyPress(c, 0, 0));

	int idx = 0;

	// set the indexes for referencing these key_presses easier
	for (int i = 0; i < 2; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			keypress_indexes[i][j] = idx;

			idx++;
		}
	}

	startTimerHz(60);
}

CameraWalkControls::~CameraWalkControls()
{
}

void CameraWalkControls::setNewCamera(Camera* c)
{
	camera = c;

	updateDirection(0.f, 0.f);
}

void CameraWalkControls::moveUp(float delta_time)
{
	camera->position += move_speed * delta_time * getRelativeUp();
}

void CameraWalkControls::moveDown(float delta_time)
{
	camera->position -= move_speed * delta_time * getRelativeUp();
}

void CameraWalkControls::moveForwardsBackwards(const juce::MouseWheelDetails& wheel)
{
	const float scrollSpeed = 2.0f; // Adjust the scroll speed as needed

	auto d = Camera::quaternionToDirection(camera->rotation);

	camera->position += (scrollSpeed * wheel.deltaY) * d;
}

void CameraWalkControls::moveLeft(float delta_time)
{
	auto d = Camera::quaternionToDirection(camera->rotation);

	camera->position -= move_speed * delta_time *
		glm::normalize(glm::cross(d, camera->up));
}

void CameraWalkControls::moveRight(float delta_time)
{
	auto d = Camera::quaternionToDirection(camera->rotation);

	camera->position += move_speed * delta_time *
		glm::normalize(glm::cross(d, camera->up));
}

void CameraWalkControls::mouseDown(const juce::MouseEvent& e)
{
	// initialize the mouse position
	last_mouse_x = e.getPosition().x;
	last_mouse_y = e.getPosition().y;
}

void CameraWalkControls::mouseDrag(const juce::MouseEvent& e)
{
	// only do angle adjustment if the middle mouse button is down
	if (e.mods.isMiddleButtonDown())
	{
		float deltaX = e.getPosition().x - last_mouse_x;
		float deltaY = e.getPosition().y - last_mouse_y;

		updateDirection(deltaX, deltaY);

		last_mouse_x = e.getPosition().x;
		last_mouse_y = e.getPosition().y;
	}
}

void CameraWalkControls::mouseWheelMove(const juce::MouseEvent& event,
	const juce::MouseWheelDetails& wheel)
{
	moveForwardsBackwards(wheel);
}

void CameraWalkControls::timerCallback()
{
	auto currentTime = juce::Time::getCurrentTime();
	auto deltaTime = (currentTime - lastUpdateTime).inSeconds();
	lastUpdateTime = currentTime;

	for (int i = 0; i < 2; i++)
	{
		if (key_presses.getUnchecked(keypress_indexes[i][0]).isCurrentlyDown())
			moveUp(deltaTime);
	}

	for (int i = 0; i < 2; i++)
	{
		if (key_presses.getUnchecked(keypress_indexes[i][1]).isCurrentlyDown())
			moveLeft(deltaTime);
	}

	for (int i = 0; i < 2; i++)
	{
		if (key_presses.getUnchecked(keypress_indexes[i][2]).isCurrentlyDown())
			moveDown(deltaTime);
	}

	for (int i = 0; i < 2; i++)
	{
		if (key_presses.getUnchecked(keypress_indexes[i][3]).isCurrentlyDown())
			moveRight(deltaTime);
	}
}

void CameraWalkControls::updateDirection(float delta_x, float delta_y)
{
	constexpr float sensitivity = juce::radiansToDegrees<float>(0.002f);

	azimuth -= delta_x * sensitivity;
	elevation += delta_y * sensitivity;

	auto d = Camera::quaternionToDirection(camera->rotation);

	if (fabs(glm::dot(d, glm::vec3(0, 1, 0))) == 1)
		elevation += 0.01;

	if (elevation < 0)
		elevation += 360;
	if (elevation > 360)
		elevation -= 360;

	if (elevation > 180)
		camera->up = glm::vec3(0, 1, 0); // facing up
	else
		camera->up = glm::vec3(0, -1, 0); // facing down

	float elev_radian = glm::radians(elevation);
	float azim_radian = glm::radians(azimuth);

	float x = std::sin(elev_radian) * std::cos(azim_radian);
	float y = std::cos(elev_radian);
	float z = std::sin(elev_radian) * std::sin(azim_radian);

	camera->rotation = Camera::directionToQuaternion(glm::normalize(glm::vec3(x, y, z)));
}

glm::vec3 CameraWalkControls::getRelativeUp()
{
	auto d = Camera::quaternionToDirection(camera->rotation);

	return glm::normalize(glm::cross(glm::cross(d, camera->up), d));
}

juce::String CameraWalkControls::serializeControlState()
{
	return juce::String();
}

void CameraWalkControls::deserializeControlState(juce::String)
{
}
