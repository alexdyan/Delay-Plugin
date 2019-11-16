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
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.

	//delay time slider
	delayTimeSlider.reset(new Slider()); //initialize the slider
	delayTimeSlider->setSliderStyle( Slider::SliderStyle::Rotary ); //make a circular slider (not a line)
	delayTimeSlider->setRange(0.0, 2000.0); //set the min and max values
	delayTimeSlider->setLookAndFeel( &LookAndFeel::getDefaultLookAndFeel() );
	addAndMakeVisible(delayTimeSlider.get()); //you need .get() because it's a unique pointer

	//feedback slider
	feedbackSlider.reset(new Slider());
	feedbackSlider->setSliderStyle( Slider::SliderStyle::Rotary );
	feedbackSlider->setRange(0.0, 1.0);
	addAndMakeVisible(feedbackSlider.get());

	delayTimeAttachment.reset( new SliderAttachment( processor.parameters, "delayTime", *delayTimeSlider.get() ) );

    setSize (400, 400);
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
    g.drawFittedText ("HIYEE HIYEE HIYEE!", getLocalBounds(), Justification::centred, 1);
}

void DelayPluginAudioProcessorEditor::resized()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..

	Rectangle<int> area = getLocalBounds();
	area.removeFromRight(getWidth()/2);

	delayTimeSlider->setBounds(area); // . accessing the unique pointer, -> accessing the thing it points to
	
	area.translate(getWidth()/2, 0);
	feedbackSlider->setBounds(area);
}
