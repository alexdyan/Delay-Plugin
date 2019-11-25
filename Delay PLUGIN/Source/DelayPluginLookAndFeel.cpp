/*
  ==============================================================================

    DelayPluginLookAndFeel.cpp
    Created: 14 Nov 2019 6:30:18pm
    Author:  Alexandra D'Yan

  ==============================================================================
*/

#include "DelayPluginLookAndFeel.h"

void DelayPluginLookAndFeel::drawRotarySlider(Graphics& g, int x, int y, int width, int height, float sliderPosProportional, float rotaryStartAngle, float rotaryEndAngle, Slider& s) {
	
	g.setColour(s.findColour(Slider::ColourIds::backgroundColourId));	//get the background color of each slider
	g.fillEllipse( s.getLocalBounds().toFloat() );						//get the bounds of the slider and fill with its color

	g.setColour(s.findColour(Slider::ColourIds::rotarySliderOutlineColourId)); //purpose to get rid of the black stroke
	g.drawEllipse(s.getLocalBounds().toFloat(), 1.0f);


	g.setColour(Colour(255, 255, 255)); //color of the tick line, applies to all sliders

	float totalAngle = rotaryEndAngle - rotaryStartAngle; //range of how far you can turn the knob
	float targetAngle = totalAngle * sliderPosProportional + rotaryStartAngle - MathConstants<float>::pi/2.f; //current knob position (or value)
	Point<int> p;
	p.setX( s.getWidth()/2 + cos(targetAngle) * s.getWidth()/2 ); //get the edge of the circle at the slider's current position (or value)
	p.setY( s.getHeight()/2 + sin(targetAngle) * s.getHeight()/2 );
	
	g.drawLine(s.getWidth() / 2, s.getHeight() / 2, p.getX(), p.getY(), 3.0f); //draw line from center of slider

}

void DelayPluginLookAndFeel::drawLinearSlider(Graphics& g, int x, int y, int width, int height, float sliderPos, float minSliderPos, float maxSliderPos, const Slider::SliderStyle, Slider& s) {

	ColourGradient modes = ColourGradient(Colour::Colour(225, 0, 0), x+25, y + height / 3, Colour::Colour(255, 0, 90), x + width-25, y + height / 3, false);
	modes.addColour(0.5, Colour::Colour(255, 0, 35));
	g.setGradientFill(modes);

	g.fillRect(x, y+height/3, width, height/3);

	g.setColour(Colour(255, 255, 255));
	g.drawRect(x, y+height/3, width/3, height/3, 2); //this position is technically mode 1

}
