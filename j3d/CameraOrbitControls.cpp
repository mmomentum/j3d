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

	camera_.position = glm::vec3(x, y, z) * zoom_distance_;

	camera_.direction = -glm::vec3(x, y, z);
}

void CameraOrbitControls::mouseDrag(const juce::MouseEvent& event)
{

}

void CameraOrbitControls::mouseWheelMove(const juce::MouseEvent& e, const juce::MouseWheelDetails& d)
{
	zoom_distance_ -= d.deltaY;

	zoom_distance_ = juce::jlimit(0.1f, 100.0f, zoom_distance_);

	DBG(zoom_distance_);

	updatePosition();
}

bool CameraOrbitControls::keyPressed(const juce::KeyPress& key, juce::Component* originatingComponent)
{
	bool return_state = false;

	if (key.getKeyCode() == juce::KeyPress::leftKey)
	{
		azimuth += 15.0f;

		return_state = true;
	}
	if (key.getKeyCode() == juce::KeyPress::rightKey)
	{
		azimuth -= 15.0f;

		return_state = true;
	}
	if (key.getKeyCode() == juce::KeyPress::upKey)
	{
		elevation += 10.0f;

		return_state = true;
	}
	if (key.getKeyCode() == juce::KeyPress::downKey)
	{
		elevation -= 10.0f;

		return_state = true;
	}

	elevation = std::clamp(elevation, 0.01f, 180.0f);
	while (azimuth > 360.0)
		azimuth -= 360.0;
	while (azimuth < 0)
		azimuth += 360.0;

	updatePosition();

	return return_state;
}
