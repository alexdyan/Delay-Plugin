/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

DelayPluginAudioProcessorEditor::DelayPluginAudioProcessorEditor (DelayPluginAudioProcessor& p)
    : AudioProcessorEditor (&p), processor (p)
{
	processor.parameters.addParameterListener("delayMode", this); //this is the listener

	//delay time slider
	delayTimeSlider.reset(new Slider());							//initialize the slider
	delayTimeSlider->setSliderStyle( Slider::SliderStyle::Rotary );	//make a circular slider (not a line)
	//colors
	delayTimeSlider->setColour(Slider::ColourIds::backgroundColourId, Colour(225, 0, 0));
	delayTimeSlider->setColour(Slider::ColourIds::rotarySliderOutlineColourId, Colour(225, 0, 0));
	addAndMakeVisible(delayTimeSlider.get());						//you need .get() because it's a unique pointer
	//label
	delayTimeLabel.reset(new Label("delayTimeLabel", "Delay Time"));
	addAndMakeVisible(delayTimeLabel.get());


	//lfo frequency slider
	lfoFreqSlider.reset(new Slider());
	lfoFreqSlider->setSliderStyle(Slider::SliderStyle::Rotary);
	//colors
	lfoFreqSlider->setColour(Slider::ColourIds::backgroundColourId, Colour(255, 0, 35));
	lfoFreqSlider->setColour(Slider::ColourIds::rotarySliderOutlineColourId, Colour(255, 0, 35));
	addAndMakeVisible(lfoFreqSlider.get());
	//label
	lfoFreqLabel.reset(new Label("lfoFreqLabel", "LFO Frequency"));
	addAndMakeVisible(lfoFreqLabel.get());


	//amplitude modulation slider
	amplitudeSlider.reset(new Slider());
	amplitudeSlider->setSliderStyle(Slider::SliderStyle::Rotary);
	//colors
	amplitudeSlider->setColour(Slider::ColourIds::backgroundColourId, Colour(255, 0, 100));
	amplitudeSlider->setColour(Slider::ColourIds::rotarySliderOutlineColourId, Colour(255, 0, 100));
	addAndMakeVisible(amplitudeSlider.get());
	//label
	amplitudeLabel.reset(new Label("amplitudeLabel", "Amplitude Threshold"));
	addAndMakeVisible(amplitudeLabel.get());


	//feedback slider
	feedbackSlider.reset(new Slider());
	feedbackSlider->setSliderStyle( Slider::SliderStyle::Rotary );
	//colors
	feedbackSlider->setColour(Slider::ColourIds::backgroundColourId, Colour(255, 0, 160));
	feedbackSlider->setColour(Slider::ColourIds::rotarySliderOutlineColourId, Colour(255, 0, 160));
	addAndMakeVisible(feedbackSlider.get());
	//label
	feedbackLabel.reset(new Label("feedbackLabel", "Feedback"));
	addAndMakeVisible(feedbackLabel.get());


	//delayMode slider (toggle)
	delayModeSlider.reset(new Slider());
	delayModeSlider->setColour(Slider::ColourIds::thumbColourId, Colour(255, 255, 255));
	addAndMakeVisible(delayModeSlider.get());
	//label
	delayModeLabel.reset(new Label("delayModeLabel", "Delay Mode"));
	addAndMakeVisible(delayModeLabel.get());


	//delay display
	display.reset(new DelayDisplay(p, processor.currentDelayTime)); //give the constructor the processor reference (p)
	addAndMakeVisible(display.get());

	//attachments
	delayTimeAttachment.reset( new SliderAttachment( processor.parameters, "delayTime", *delayTimeSlider.get() ) );
	lfoFreqAttachment.reset(new SliderAttachment(processor.parameters, "lfoFrequency", *lfoFreqSlider.get()));
	amplitudeAttachment.reset(new SliderAttachment(processor.parameters, "amplitudeThresh", *amplitudeSlider.get()));
	feedbackAttachment.reset( new SliderAttachment( processor.parameters, "feedback", *feedbackSlider.get() ) );
	delayModeAttachment.reset( new SliderAttachment( processor.parameters, "delayMode", *delayModeSlider.get() ) );

	//always set the size at the end of the constructor
    setSize (600, 600);
}

DelayPluginAudioProcessorEditor::~DelayPluginAudioProcessorEditor()
{
}

////////////////////////////////////////////////////////////////////////////////

void DelayPluginAudioProcessorEditor::paint (Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (ResizableWindow::backgroundColourId));

    g.setColour (Colours::white);
    g.setFont (15.0f);

}

void DelayPluginAudioProcessorEditor::resized()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..
	// NOTE FOR ME . accessing the unique pointer, -> accessing the thing it points to

	Rectangle<int> displayArea = getLocalBounds();
	displayArea.removeFromBottom(getHeight()/2);
	display->setBounds(displayArea); //display on the top half of window

	Rectangle<int> feedbackArea = getLocalBounds();
	feedbackArea.removeFromTop(getHeight() / 2);
	feedbackArea = feedbackArea.removeFromRight(getWidth() / 4);
	feedbackArea = feedbackArea.withSizeKeepingCentre(feedbackArea.getWidth(), feedbackArea.getWidth());
	feedbackArea.translate(getWidth() * -0.01, 0);
	feedbackSlider->setBounds(feedbackArea); //feedback slider on the right 1/4 of window

	Rectangle<int> delayModeArea = getLocalBounds();
	delayModeArea.removeFromTop(getHeight() / 2);
	delayModeArea.removeFromRight(getWidth() / 4);
	delayModeArea = delayModeArea.removeFromTop( (getHeight()/2) / 3 );
	delayModeArea = delayModeArea.withSizeKeepingCentre(delayModeArea.getWidth() * 0.8, delayModeArea.getHeight());
	delayModeSlider->setBounds(delayModeArea);

	Rectangle<int> delayArea1 = getLocalBounds();
	delayArea1 = delayArea1.removeFromBottom(getHeight()/3);
	delayArea1 = delayArea1.removeFromLeft(getWidth() / 4);
	delayArea1 = delayArea1.withSizeKeepingCentre(delayArea1.getWidth() * 0.8, delayArea1.getWidth() * 0.8);
	delayTimeSlider->setBounds(delayArea1); //manual delay slider on the left 1/4 of window

	delayArea1.translate(getWidth() / 4, 0);
	lfoFreqSlider->setBounds(delayArea1); //lfo frequency slider on the left 2/4 of window

	delayArea1.translate(getWidth() / 4, 0);
	amplitudeSlider->setBounds(delayArea1); //amplitude threshold slider on right 2/4 of window


	//label positioning
	Rectangle<int> labelArea = getLocalBounds();
	labelArea = labelArea.removeFromBottom(getHeight() / 14);
	labelArea = labelArea.removeFromLeft(getWidth() / 4);
	delayTimeLabel->setJustificationType(Justification::centred);
	delayTimeLabel->setBounds(labelArea); //delay time label

	labelArea.translate(getWidth() / 4, 0);
	lfoFreqLabel->setJustificationType(Justification::centred);
	lfoFreqLabel->setBounds(labelArea); //lfo frequency label

	labelArea.translate(getWidth() / 4, 0);
	amplitudeLabel->setJustificationType(Justification::centred);
	amplitudeLabel->setBounds(labelArea); //amplitude modulation label DOESNT WORK

	labelArea.translate(getWidth() / 4, getHeight() / -16);
	feedbackLabel->setJustificationType(Justification::centred);
	feedbackLabel->setBounds(labelArea); //feedback label

}

void DelayPluginAudioProcessorEditor::parameterChanged(const String& parameterId, float newParameterValue) {
	String labelText;

	//you have to do this bc you can't do gui stuff on the audio thread bc it will yell at you and you're calling this asynchronously to avoid that
	if (newParameterValue == 1) { //manual delay mode
		auto updateLabel = [&] { //lambda function
			delayTimeLabel->setText("Delay Time", NotificationType::dontSendNotification);
		};

		MessageManager::callAsync(updateLabel); //call asynchronously
	}

	else { //lfo and amplitude modes
		auto updateLabel = [&] {
			delayTimeLabel->setText("Max Delay Time", NotificationType::dontSendNotification);
		};

		MessageManager::callAsync(updateLabel);
	}
}