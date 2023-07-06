#pragma once

#include <JuceHeader.h>

#include "../../j3d/GLRenderer.h"
#include <glm.hpp>

class TestSceneBase : public j3d::GLRenderer
{
	class TestParameter
	{
	public:
		TestParameter(juce::String name, float min_value, float max_value, float default_value);

		float getValue();
		void setValue(float value);

		juce::String getName();

		juce::NormalisableRange<float> getRange();

		class Listener
		{
		public:
			virtual ~Listener() {};
			virtual void parameterUpdated(juce::String param_name) = 0;
		};

		void addListener(Listener* listener);
		void removeListener(Listener* listener);

	private:
		float value = 0.0f;
		float default_value = 0.0f;

		juce::String name;

		juce::NormalisableRange<float> range;

		juce::ListenerList<Listener> listeners;

		JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TestParameter)
	};

	class ParameterComponent : public juce::Component
	{
	public:
		ParameterComponent(TestParameter& param_ref);
		~ParameterComponent() override;

		void paint(juce::Graphics&) override;
		void resized() override;

	private:
		juce::Label name;
		juce::Slider slider;

		TestParameter& owner;
	};

public:
	TestSceneBase();
	~TestSceneBase();

	void addTestParameter(juce::String name, float min_value, float max_value, float default_value);

	TestParameter& getTestParameter(juce::String parameter_name);
	TestParameter& getTestParameter(int param_idx);

	float getParameterValue(juce::String parameter_name);
	float getParameterValue(int param_idx);

	void resized() override;
	void paint(juce::Graphics&) override;

private:
	std::vector<std::unique_ptr<TestParameter>> parameters;
	std::vector<std::unique_ptr<ParameterComponent>> sliders;
};