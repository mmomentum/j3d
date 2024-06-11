#include "CameraControlsBase.h"

CameraControlsBase::CameraControlsBase(Camera* c, juce::Component& o) : camera(c), owner(o)
{
	juce::MessageManagerLock lock;

	if (lock.lockWasGained())
	{
		owner.addKeyListener(this);
		owner.addMouseListener(this, true);
		owner.setWantsKeyboardFocus(true);
	}
}

CameraControlsBase::~CameraControlsBase()
{
	juce::MessageManagerLock lock;

	if (lock.lockWasGained())
	{
		owner.removeKeyListener(this);
		owner.removeMouseListener(this);
	}
}