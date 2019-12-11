/*
 ==============================================================================
 
 Delay.cpp
 Created: 7 Dec 2019 7:00:02pm
 Author:  Alexandra D'Yan
 
 ==============================================================================
 */

#include "Delay.h"
#include "PluginProcessor.h"

Delay::Delay(DelayPluginAudioProcessor &p) : processor(p)
{
    
}

void Delay::prepareToPlay (double sampleRate)
{
    lastSampleRate = sampleRate;
    delayBuffer.setSize(2, sampleRate * 5);
    delayBuffer.clear();
    
}

void Delay::processBlock(AudioBuffer<float>& buffer)
{
    float gain = 0.8;
    float time = processor.currentDelayTime;
    //DBG(time);
    float feedback = *processor.parameters.getRawParameterValue("feedback");
    
	//write dry signal to delay buffer
    for (int i = 0; i < 2; ++i) {
        writeToDelayBuffer(buffer, i, i, writePosition, 1.0f, 1.0f, true);
    }
    
	//overall gain
    buffer.applyGainRamp(0, buffer.getNumSamples(), lastInputGain, gain);
    lastInputGain = gain;
    
    //read delayed signal
    auto readPos = roundToInt(writePosition - (lastSampleRate * time / 1000.0));
	if (readPos < 0) {
		readPos += delayBuffer.getNumSamples();
	}
    
    //if it's already run at least once
    if (nextReadPosition >= 0) {
        //fade out
        auto endGain = (readPos == nextReadPosition) ? 1.0f : 0.0f;
        for (int i=0; i<buffer.getNumChannels(); ++i) {
            readFromDelayBuffer (buffer, i, i, nextReadPosition, 1.0, endGain, false);
        }
    }
    
	//fade in
    if (readPos != nextReadPosition) {
        for (int i=0; i<buffer.getNumChannels(); ++i) {
            readFromDelayBuffer (buffer, i, i, readPos, 0.0, 1.0, false);
        }
    }
    
	//apply feedback to delayed signal
    for (int i=0; i<buffer.getNumChannels(); ++i) {
        writeToDelayBuffer (buffer, i, i, writePosition, lastFeedbackGain, feedback, false);
    }
    lastFeedbackGain = feedback;
    
	//update write position
    writePosition += buffer.getNumSamples();
	if (writePosition >= delayBuffer.getNumSamples()) {
		writePosition -= delayBuffer.getNumSamples();
	}
    
	nextReadPosition = readPos + buffer.getNumSamples();
	if (nextReadPosition >= delayBuffer.getNumSamples()) {
		nextReadPosition -= delayBuffer.getNumSamples();
	}

}

void Delay::writeToDelayBuffer (AudioBuffer<float> &buffer, const int channelIn, const int channelOut, const int writePos, float startGain, float endGain, bool replacing)
{
	int numSamples = buffer.getNumSamples();
	int delaySamples = delayBuffer.getNumSamples();

	if (writePos + numSamples <= delaySamples) {
        if (replacing)
            delayBuffer.copyFromWithRamp (channelOut, writePos, buffer.getReadPointer (channelIn), numSamples, startGain, endGain);
        else
            delayBuffer.addFromWithRamp (channelOut, writePos, buffer.getReadPointer (channelIn), numSamples, startGain, endGain);
    }

    else { //if you exceed length of delay buffer
        auto midPos  = delaySamples - writePos; //number of samples left until the end of delaybuffer
		auto leftOverSamples = numSamples - midPos;	//number of samples that get cut off and have to be put at the beginning
        auto midGain = jmap (float (midPos) / numSamples, startGain, endGain);

        if (replacing) {
            delayBuffer.copyFromWithRamp (channelOut, writePos, buffer.getReadPointer (channelIn), midPos, lastInputGain, midGain);
            delayBuffer.copyFromWithRamp (channelOut, 0, buffer.getReadPointer (channelIn, midPos), leftOverSamples, midGain, endGain);
        }
        else {
            delayBuffer.addFromWithRamp (channelOut, writePos, buffer.getReadPointer (channelIn), midPos, lastInputGain, midGain);
            delayBuffer.addFromWithRamp (channelOut, 0, buffer.getReadPointer (channelIn, midPos), leftOverSamples, midGain, endGain);
        }
    }

}

void Delay::readFromDelayBuffer (AudioBuffer<float>& buffer, const int channelIn, const int channelOut, const int readPos, float startGain, float endGain, bool replacing)
{
	int numSamples = buffer.getNumSamples();
	int delaySamples = delayBuffer.getNumSamples();

	//wrap around from end of last buffer to start of next one
	//delayTime is ms and fs is in seconds -> math to compensate
	//mod by delaybuffer length to wrap around when near the end of buffer

    if (readPos + numSamples <= delaySamples) {
        if (replacing)
            buffer.copyFromWithRamp (channelOut, 0, delayBuffer.getReadPointer (channelIn, readPos), numSamples, startGain, endGain);
        else
            buffer.addFromWithRamp (channelOut, 0, delayBuffer.getReadPointer (channelIn, readPos), numSamples, startGain, endGain);
    }

    else { //if you exceed length of delay buffer
        auto midPos  = delaySamples - readPos; //number of samples left until the end of delaybuffer
		auto leftOverSamples = numSamples - midPos;	//number of samples that get cut off and have to be put at the beginning
        auto midGain = jmap (float (midPos) / numSamples, startGain, endGain);

        if (replacing) {
            buffer.copyFromWithRamp (channelOut, 0, delayBuffer.getReadPointer (channelIn, readPos), midPos, startGain, midGain);
            buffer.copyFromWithRamp (channelOut, midPos, delayBuffer.getReadPointer (channelIn), leftOverSamples, midGain, endGain);
        }
        else {
            buffer.addFromWithRamp (channelOut, 0, delayBuffer.getReadPointer (channelIn, readPos), midPos, startGain, midGain);
            buffer.addFromWithRamp (channelOut, midPos, delayBuffer.getReadPointer (channelIn), leftOverSamples, midGain, endGain);
        }
    }
}

