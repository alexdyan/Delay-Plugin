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

//==============================================================================
/*
*/
class DelayDisplay    : public Component
{
public:
	enum ColourIds :int {
		backgroundColourId = 1,
		mainWaveColourId,
		delayedWaveColourId
	};
    DelayDisplay(DelayPluginAudioProcessor& processor);
    ~DelayDisplay();

    void paint (Graphics&) override;
	void updateDelayTime(); //set the gui delayTime variable to the actual slider value

private:
	// This reference is provided as a quick way for your editor to
	// access the processor object that created it.
	DelayPluginAudioProcessor& processor;

	float delayTime = 500;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (DelayDisplay)
};
