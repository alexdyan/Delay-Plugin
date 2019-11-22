/*
  ==============================================================================

    LFO.cpp
    Created: 22 Nov 2019 12:49:59pm
    Author:  Alexandra D'Yan

  ==============================================================================
*/

#include "LFO.h"
#include "PluginProcessor.h"

LFO::LFO(DelayPluginAudioProcessor& p) : processor(p) {
	
}

void LFO::updateAngleDelta() {
	lfoFrequency = *processor.parameters.getRawParameterValue("lfoFrequency");

	auto cyclesPerSample = lfoFrequency / currentSampleRate; //how many cycles of your lfo are completed every sample
	angleDelta = cyclesPerSample * 2.0 * MathConstants<double>::pi; //multiply by 2pi to get the angle of how much you should increment every sample (like get the value of this sin wave at pi instead of at sample 24000)
}

void LFO::prepareToPlay(double sampleRate) {
	currentSampleRate = sampleRate;
	updateAngleDelta();
}

void LFO::processBlock(AudioBuffer<float>& buffer) {
	auto* bufferData = buffer.getWritePointer(0);

	for (int i = 0; i < buffer.getNumSamples(); i++) {
		float currentSample = (float)std::sin(currentAngle);
		currentAngle += angleDelta;
		bufferData[i] = currentSample;
	}
}