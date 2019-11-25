/*
  ==============================================================================

    DelayDisplay.h
    Created: 20 Nov 2019 11:51:56am
    Author:  Alexandra D'Yan

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "PluginProcessor.h"


class DelayDisplay : public Component, public Timer, AudioProcessorValueTreeState::Listener //inherit from the audio tree listener (to access the delay time in the gui)
{
public:
	enum ColourIds :int {
		backgroundColourId = 1,
		mainWaveColourId,
		delayedWaveColourId
	};
    DelayDisplay(DelayPluginAudioProcessor& processor, float &currentDelayTime);
    ~DelayDisplay();

    void paint (Graphics&) override;
	void updateDelayTime(); //set the gui delayTime variable to the actual slider value
	void parameterChanged(const String& parameterId, float newParameterValue);
	void timerCallback() override;

private:
	// This reference is provided as a quick way for your editor to
	// access the processor object that created it.
	DelayPluginAudioProcessor& processor;

	float delayTime = 500;
	float lfoFreq = 0.1;
	float *currentDelayTime = 0;
	float lastDelayTime = 0;
	std::unique_ptr<DelayDisplay> display;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (DelayDisplay)
};
