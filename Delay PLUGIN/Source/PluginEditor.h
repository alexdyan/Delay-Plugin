/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "PluginProcessor.h"
#include "DelayDisplay.h"
typedef AudioProcessorValueTreeState::SliderAttachment SliderAttachment; //the thing for attaching the slider info to the layout


class DelayPluginAudioProcessorEditor : public AudioProcessorEditor, AudioProcessorValueTreeState::Listener
{
public:
    DelayPluginAudioProcessorEditor (DelayPluginAudioProcessor&);
    ~DelayPluginAudioProcessorEditor();

    void paint (Graphics&) override;
    void resized() override;
	void parameterChanged(const String& parameterId, float newParameterValue);

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    DelayPluginAudioProcessor& processor;

	std::unique_ptr<Slider> delayTimeSlider;
	std::unique_ptr<SliderAttachment> delayTimeAttachment; //how you link the gui elements to the actual delay variables
	std::unique_ptr<Label> delayTimeLabel;

	std::unique_ptr<Slider> lfoFreqSlider;
	std::unique_ptr<SliderAttachment> lfoFreqAttachment;
	std::unique_ptr<Label> lfoFreqLabel;

	std::unique_ptr<Slider> amplitudeSlider;
	std::unique_ptr<SliderAttachment> amplitudeAttachment;
	std::unique_ptr<Label> amplitudeLabel;

	std::unique_ptr<Slider> feedbackSlider;
	std::unique_ptr<SliderAttachment> feedbackAttachment;
	std::unique_ptr<Label> feedbackLabel;

	std::unique_ptr<Slider> delayModeSlider;
	std::unique_ptr<SliderAttachment> delayModeAttachment;
	std::unique_ptr<Label> delayModeLabel;

	std::unique_ptr<DelayDisplay> display;
    
    TextButton button;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (DelayPluginAudioProcessorEditor)
};
