/*
  ==============================================================================

    DelayDisplay.cpp
    Created: 20 Nov 2019 11:51:56am
    Author:  Alexandra D'Yan

  ==============================================================================
*/

#include "../JuceLibraryCode/JuceHeader.h"
#include "DelayDisplay.h"

//==============================================================================
DelayDisplay::DelayDisplay(DelayPluginAudioProcessor& p):processor(p) //copy the reference to p
{
    // In your constructor, you should add any child components, and
    // initialise any special settings that your component needs.

	setColour(ColourIds::backgroundColourId, Colours::black);
	setColour(ColourIds::mainWaveColourId, Colours::pink);
	setColour(ColourIds::delayedWaveColourId, Colours::red);
}

DelayDisplay::~DelayDisplay()
{
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
	g.strokePath(p, PathStrokeType(2.f)); //path thickness

	p.applyTransform(AffineTransform::translation(delayTime/10, 0));
	g.setColour(findColour(ColourIds::delayedWaveColourId));
	g.strokePath(p, PathStrokeType(2.f)); //path thickness

}

void DelayDisplay::updateDelayTime() {
	//gui world delayTime variable
	delayTime = *processor.parameters.getRawParameterValue("delayTime");
	repaint();
}