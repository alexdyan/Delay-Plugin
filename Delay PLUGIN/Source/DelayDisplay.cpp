/*
  ==============================================================================

    DelayDisplay.cpp
    Created: 20 Nov 2019 11:51:56am
    Author:  Alexandra D'Yan

  ==============================================================================
*/

#include "../JuceLibraryCode/JuceHeader.h"
#include "DelayDisplay.h"

DelayDisplay::DelayDisplay(DelayPluginAudioProcessor& p, float &curDelayTime):processor(p) //copy the reference to p
{
    // In your constructor, you should add any child components, and
    // initialise any special settings that your component needs.

	processor.parameters.addParameterListener("delayTime", this); //these are the listeners
	processor.parameters.addParameterListener("delayMode", this);
	currentDelayTime = &curDelayTime;

	setColour(ColourIds::backgroundColourId, Colours::black);
	setColour(ColourIds::mainWaveColourId, Colours::grey);
	setColour(ColourIds::delayedWaveColourId, Colours::red);

	lastDelayTime = *processor.parameters.getRawParameterValue("delayTime");
}

DelayDisplay::~DelayDisplay()
{
	processor.parameters.removeParameterListener("delayTime", this);
	processor.parameters.removeParameterListener("delayMode", this);
}

void DelayDisplay::paint (Graphics& g)
{
	//	float controlPointX = JUCE_LIVE_CONSTANT(34.5); cool for testing purposes
	//stack allocated memory, "new" is heap allocated memory (pointers)

	g.fillAll(findColour(ColourIds::backgroundColourId));
	Path p;
	p.startNewSubPath(0, getHeight() / 2); //start wave from the middle left side of the display area

	//draw the path to make the MAIN waveform
	int upDown = -1;
	for (int i = 0; i < 6; i++) {
		float xDist = getWidth() / 5.f;
		float y = getHeight() / 2.f;
		float yDist = getHeight() * 0.4;
		upDown *= -1;

		float endPointX = xDist * i;
		float endPointY = y;
		float controlPointX = endPointX - xDist / 2.0;
		float controlPointY = yDist + upDown * yDist;
		p.quadraticTo(controlPointX, controlPointY, endPointX, endPointY);
	}
	g.setColour(findColour(ColourIds::mainWaveColourId));
	g.strokePath(p, PathStrokeType(2.5f)); //path thickness

	p.applyTransform(AffineTransform::translation(lastDelayTime/10, 0));
	g.setColour(findColour(ColourIds::delayedWaveColourId));
	g.strokePath(p, PathStrokeType(2.5f)); //path thickness

}

void DelayDisplay::updateDelayTime() {
	//gui world delayTime variable
	lastDelayTime = *processor.parameters.getRawParameterValue("delayTime");
	repaint();
}

//this part is so we can access delayTime from the gui world, but don't know how/why it works
//see updateDelayTime in DelayDisplay.cpp
void DelayDisplay::parameterChanged(const String& parameterId, float newParameterValue) {

	if (parameterId.equalsIgnoreCase("delayTime")) {
		auto updateDisplay = [&] { //lambda -> function that's also a variable or something
			updateDelayTime(); //set the gui delayTime to the actual slider delayTime value
		};
		MessageManager::callAsync(updateDisplay); //don't know
	}

	else if (parameterId.equalsIgnoreCase("delayMode")) {
		if (newParameterValue > 1) { //lfo and amplitude mode
			startTimerHz(30);
		}
		if (newParameterValue == 1) {
			stopTimer();
		}
	}
}

void DelayDisplay::timerCallback() {

	if (lastDelayTime != *currentDelayTime) {
		lastDelayTime = *currentDelayTime;
		repaint();
	}
}