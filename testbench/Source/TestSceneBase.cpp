#include "TestSceneBase.h"

TestSceneBase::TestParameter::TestParameter(juce::String n, 
    float min_value, float max_value, float d) : name(n), default_value(d), value(d)
{
    range = juce::NormalisableRange<float>(min_value, max_value);
}

float TestSceneBase::TestParameter::getValue()
{
    return juce::jlimit(range.start, range.end, value);
}

void TestSceneBase::TestParameter::setValue(float v)
{
    value = v;

    listeners.call(&Listener::parameterUpdated, name);
}

juce::String TestSceneBase::TestParameter::getName()
{
    return name;
}

juce::NormalisableRange<float> TestSceneBase::TestParameter::getRange()
{
    return range;
}

void TestSceneBase::TestParameter::addListener(Listener* listener)
{
    listeners.add(listener);
}

void TestSceneBase::TestParameter::removeListener(Listener* listener)
{
    listeners.remove(listener);
}

TestSceneBase::ParameterComponent::ParameterComponent(TestParameter& p) : owner(p)
{
    name.setText(p.getName(), juce::dontSendNotification);
    addAndMakeVisible(&name);

    slider.setRange(owner.getRange().getRange().getStart(), 
        owner.getRange().getRange().getEnd(), 0.001f);
    slider.setValue(owner.getValue(), juce::dontSendNotification);
    slider.setSliderStyle(juce::Slider::LinearBar);
    slider.setTextBoxStyle(juce::Slider::TextEntryBoxPosition::NoTextBox, true, 0, 0);
    addAndMakeVisible(&slider);

    slider.onValueChange = [&]()
    {
        owner.setValue(slider.getValue());
    };
}

TestSceneBase::ParameterComponent::~ParameterComponent()
{

}

void TestSceneBase::ParameterComponent::paint(juce::Graphics&)
{
}

void TestSceneBase::ParameterComponent::resized()
{
    const int hdiv2 = getHeight() / 2;

    name.setBounds(0, 0, getWidth(), hdiv2);
    slider.setBounds(0, hdiv2, getWidth(), hdiv2);
}

TestSceneBase::TestSceneBase()
{

}

TestSceneBase::~TestSceneBase()
{

}

void TestSceneBase::addTestParameter(juce::String name, float min_value, float max_value, float default_value)
{
    // create parameter
    parameters.push_back(std::make_unique<TestParameter>(name, min_value, max_value, default_value));

    sliders.push_back(std::make_unique<ParameterComponent>(*parameters[parameters.size() - 1].get()));
    addAndMakeVisible(*sliders[sliders.size() - 1].get());
}

TestSceneBase::TestParameter& TestSceneBase::getTestParameter(juce::String parameter_name)
{
    for (auto& p : parameters)
    {
        if (p->getName() == parameter_name)
        {
            return *p;
        }
    }
    
    jassertfalse; // no name matched. returning an invalid parameter
    return TestParameter("invalid", 0.0f, 1.0f, 0.0f);
}

TestSceneBase::TestParameter& TestSceneBase::getTestParameter(int i)
{
    jassert(i <= 0 && i > parameters.size());

    return *parameters[i];
}

float TestSceneBase::getParameterValue(juce::String parameter_name)
{
    for (auto& p : parameters)
    {
        if (p->getName() == parameter_name)
        {
            return p->getValue();
        }
    }

    jassertfalse; // no name matched. returning zero
    return 0.0f;
}

float TestSceneBase::getParameterValue(int i)
{
    jassert(i <= 0 && i > parameters.size());

    return parameters[i]->getValue();
}

void TestSceneBase::resized()
{
    const int w = getWidth();

    const int slider_h = 40;

    const int slider_w = juce::jmin(int(w * 0.1f), 100);

    for (int i = 0; i < sliders.size(); i++)
    {
        sliders[i]->setBounds(w - slider_w, slider_h * i, slider_w, slider_h);
    }
}

void TestSceneBase::paint(juce::Graphics&)
{

}