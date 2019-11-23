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
	processor.parameters.addParameterListener("delayTime", this); //this is the listener

	//delay time slider
	delayTimeSlider.reset(new Slider()); //initialize the slider
	delayTimeSlider->setSliderStyle( Slider::SliderStyle::Rotary ); //make a circular slider (not a line)
	//delayTimeSlider->setRange(0.0, 2000.0); //set the min and max values (don't have to use this bc when you use the attachment, the parameter variable sets the range)
	delayTimeSlider->setLookAndFeel( &LookAndFeel::getDefaultLookAndFeel() );
	addAndMakeVisible(delayTimeSlider.get()); //you need .get() because it's a unique pointer

	//feedback slider
	feedbackSlider.reset(new Slider());
	feedbackSlider->setSliderStyle( Slider::SliderStyle::Rotary );
	addAndMakeVisible(feedbackSlider.get());

	//lfo frequency slider
	lfoFreqSlider.reset(new Slider());
	lfoFreqSlider->setSliderStyle(Slider::SliderStyle::Rotary);
	addAndMakeVisible(lfoFreqSlider.get());

	//delayMode slider (toggle)
	delayModeSlider.reset(new Slider());
	addAndMakeVisible(delayModeSlider.get());

	//delay display
	display.reset(new DelayDisplay(p)); //give the constructor the processor reference (p)
	addAndMakeVisible(display.get());

	//attachments
	delayTimeAttachment.reset( new SliderAttachment( processor.parameters, "delayTime", *delayTimeSlider.get() ) );
	feedbackAttachment.reset( new SliderAttachment( processor.parameters, "feedback", *feedbackSlider.get() ) );
	lfoFreqAttachment.reset( new SliderAttachment( processor.parameters, "lfoFrequency", *lfoFreqSlider.get() ) );
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
	// NOTE . accessing the unique pointer, -> accessing the thing it points to

	Rectangle<int> displayArea = getLocalBounds();
	displayArea.removeFromBottom(getHeight()/2);
	display->setBounds(displayArea); //display on the top half of window

	Rectangle<int> feedbackArea = getLocalBounds();
	feedbackArea.removeFromTop(getHeight() / 2);
	feedbackArea = feedbackArea.removeFromRight(getWidth() / 4);
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
	delayTimeSlider->setBounds(delayArea1); //manual delay slider on the left 1/4 of window

	delayArea1.translate(getWidth() / 4, 0);
	lfoFreqSlider->setBounds(delayArea1); //lfo frequency slider on the left 2/4 of window

	delayArea1.translate(getWidth() / 4, 0);
	//this is the space for amplitude modulated delay
	
}

//this part is so we can access delayTime from the gui world, but don't know how/why it works
//see updateDelayTime in DelayDisplay.cpp
void DelayPluginAudioProcessorEditor::parameterChanged(const String &parameterId, float newParameterValue) {
	if (parameterId.equalsIgnoreCase("delayTime")) {
		auto updateDisplay = [&] { //lambda -> function that's also a variable or something
			display->updateDelayTime(); //set the gui delayTime to the actual slider delayTime value
		};
		MessageManager::callAsync(updateDisplay); //don't know
	}
}