/*
  ==============================================================================

    Delay.h
    Created: 7 Dec 2019 7:00:02pm
    Author:  Alexandra D'Yan

  ==============================================================================
*/

#pragma once
#include "JuceHeader.h"

class DelayPluginAudioProcessor;

class Delay
{
public:
    
    Delay(DelayPluginAudioProcessor& );
    
    void prepareToPlay (double sampleRate);
    void processBlock(AudioBuffer<float> &buffer);
    
    void writeToDelayBuffer (AudioBuffer<float>& buffer, const int channelIn, const int channelOut, const int writePos, float startGain, float endGain, bool replacing);
    void readFromDelayBuffer (AudioBuffer<float>& buffer, const int channelIn, const int channelOut, const int readPos, float startGain, float endGain, bool replacing);

    int getLengthInSamples() { return delayBuffer.getNumSamples(); }
    
    DelayPluginAudioProcessor &processor;
    
private:
    AudioBuffer<float> delayBuffer;

    float lastInputGain = 0.0;
    float lastFeedbackGain = 0.0;

    int    writePosition = 0;
    int    nextReadPosition = -1;
    double lastSampleRate = 0;


};
