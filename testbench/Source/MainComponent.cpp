#include "MainComponent.h"

//==============================================================================
MainComponent::MainComponent()
{
    clip_plane_test = std::make_unique<ClipPlaneTest>();
    addAndMakeVisible(clip_plane_test.get());

    setSize(600, 400);
}

MainComponent::~MainComponent()
{
}

//==============================================================================
void MainComponent::paint (juce::Graphics& g)
{

}

void MainComponent::resized()
{
    clip_plane_test->setBounds(getBounds());
}
