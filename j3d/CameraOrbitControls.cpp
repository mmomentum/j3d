#include "CameraOrbitControls.h"

CameraOrbitControls::CameraOrbitControls(camera& cam, juce::Component& comp) : camera_(cam), component_scene_(comp)
{
	juce::MessageManagerLock lock;

	comp.addKeyListener(this);
	comp.addMouseListener(this, true);

	updatePosition();
}

void CameraOrbitControls::updatePosition()
{
	float elev_radian = glm::radians(elevation);
	float azim_radian = glm::radians(azimuth);

	float x = std::sin(elev_radian) * std::cos(azim_radian);
	float y = std::cos(elev_radian);
	float z = std::sin(elev_radian) * std::sin(azim_radian);

	camera_.position = glm::vec3(x, y, z) * zoom_distance_ + origin_point;
	camera_.direction = -glm::vec3(x, y, z);
}

void CameraOrbitControls::mouseDown(const juce::MouseEvent& e)
{
	lastMousePosition = e.getPosition();
	lastCameraPosition = origin_point;
}

void CameraOrbitControls::mouseDrag(const juce::MouseEvent& e)
{
	// orbit
	if (e.mods.isMiddleButtonDown())
	{
		const float scale = 0.5f;

		azimuth = azimuth + (scale *
			(e.getPosition().getX() - lastMousePosition.getX()));

		elevation = elevation - (scale *
			(e.getPosition().getY() - lastMousePosition.getY()));

		lastMousePosition = e.getPosition();

		updatePosition();

		return;
	}
	// pan
	if (e.mods.isRightButtonDown())
	{
		glm::vec2 currentMousePos(
			e.getPosition().getX() / component_scene_.getWidth(),
			e.getPosition().getY() / component_scene_.getHeight());

		glm::vec2 mouseOffset = currentMousePos - glm::vec2(lastMousePosition.x, lastMousePosition.y);

		// Adjust the camera's origin based on the mouse offset
		origin_point = lastCameraPosition - glm::vec3(mouseOffset.x, mouseOffset.y, 0.0f);

		updatePosition();

		return;
	}
}

void CameraOrbitControls::mouseWheelMove(const juce::MouseEvent& e, const juce::MouseWheelDetails& d)
{
	zoom_distance_ -= d.deltaY;

	zoom_distance_ = juce::jlimit(0.1f, 100.0f, zoom_distance_);

	updatePosition();
}

bool CameraOrbitControls::keyPressed(const juce::KeyPress& key, juce::Component* originatingComponent)
{
	bool return_state = false;

	return return_state;
}
