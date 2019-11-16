/*
  ==============================================================================

    DelayPluginLookAndFeel.cpp
    Created: 14 Nov 2019 6:30:18pm
    Author:  Alexandra D'Yan

  ==============================================================================
*/

#include "DelayPluginLookAndFeel.h"

void DelayPluginLookAndFeel::drawRotarySlider(Graphics& g, int x, int y, int width, int height, float sliderPosProportional, float rotaryStartAngle, float rotaryEndAngle, Slider& s) {
	g.setColour(Colour(255, 0, 175)); //set a new color
	g.fillEllipse( s.getLocalBounds().toFloat() ); //get the bounds of the slider (knob) and fill with current color

	g.setColour(Colour(0, 0, 0));

	float totalAngle = rotaryEndAngle - rotaryStartAngle; //range of how far you can turn the knob
	float targetAngle = totalAngle * sliderPosProportional + rotaryStartAngle - MathConstants<float>::pi/2.f; //current knob position (or value)
	Point<int> p;
	p.setX( s.getWidth()/2 + cos(targetAngle) * s.getWidth()/2 ); //get the edge of the circle at the slider's current position (or value)
	p.setY( s.getHeight()/2 + sin(targetAngle) * s.getHeight()/2 );

	//DBG(sliderPosProportional);
	
	g.drawLine(s.getWidth() / 2, s.getHeight() / 2, p.getX(), p.getY()); //draw line from center of slider

}
