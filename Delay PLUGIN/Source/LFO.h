/*
  ==============================================================================

    LFO.h
    Created: 22 Nov 2019 12:49:59pm
    Author:  Alexandra D'Yan

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
class DelayPluginAudioProcessor; //forward declaration -> let it know that there will be a class named this later on

class LFO {
	public:
		LFO(DelayPluginAudioProcessor &p);
		void updateAngleDelta();
		void prepareToPlay(double sampleRate);
		void processBlock(AudioBuffer<float>& buffer);

	private:
		DelayPluginAudioProcessor &processor;
		float lfoFrequency = 1.0;
		double currentSampleRate = 0.0; //using double for accuracy
		double angleDelta = 0.0;
		double currentAngle = 0.0;
};