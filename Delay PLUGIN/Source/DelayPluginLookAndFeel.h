/*
  ==============================================================================

    DelayPluginLookAndFeel.h
    Created: 14 Nov 2019 6:30:18pm
    Author:  Alexandra D'Yan

  ==============================================================================
*/

#pragma once
#include "../JuceLibraryCode/JuceHeader.h"

class DelayPluginLookAndFeel : public LookAndFeel_V4 {

public:
	virtual void drawRotarySlider(Graphics&, int x, int y, int width, int height, float sliderPosProportional, float rotaryStartAngle, float rotaryEndAngle, Slider&) override;
	virtual void drawLinearSlider(Graphics&, int x, int y, int width, int height, float sliderPos, float minSliderPos, float maxSliderPos, const Slider::SliderStyle, Slider&) override;
};