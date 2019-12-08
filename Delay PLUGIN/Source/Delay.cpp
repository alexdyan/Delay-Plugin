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
    mSampleRate = sampleRate;
    mDelayBuffer.setSize(2, sampleRate * 5);
    mDelayBuffer.clear();
    
}

void Delay::processBlock(AudioBuffer<float>& buffer)
{
    const float gain = .8;
    const float time = processor.currentDelayTime;
    DBG(time);
    const float feedback = *processor.parameters.getRawParameterValue("feedback");
    
    // write original to delay
    for (int i = 0; i < 2; ++i)
    {
        writeToDelayBuffer (buffer, i, i, mWritePos, 1.0f, 1.0f, true);
    }
    
    // adapt dry gain
    buffer.applyGainRamp (0, buffer.getNumSamples(), mLastInputGain, gain);
    mLastInputGain = gain;
    
    // read delayed signal
    auto readPos = roundToInt (mWritePos - (mSampleRate * time / 1000.0));
    if (readPos < 0)
        readPos += mDelayBuffer.getNumSamples();
    
    // if has run before
    if (mExpectedReadPos >= 0)
    {
        // fade out if readPos is off
        auto endGain = (readPos == mExpectedReadPos) ? 1.0f : 0.0f;
        for (int i=0; i<buffer.getNumChannels(); ++i)
        {
            readFromDelayBuffer (buffer, i, i, mExpectedReadPos, 1.0, endGain, false);
        }
    }
    
    // fade in at new position
    if (readPos != mExpectedReadPos)
    {
        for (int i=0; i<buffer.getNumChannels(); ++i)
        {
            readFromDelayBuffer (buffer, i, i, readPos, 0.0, 1.0, false);
        }
    }
    
    // add feedback to delay
    for (int i=0; i<buffer.getNumChannels(); ++i)
    {
        writeToDelayBuffer (buffer, i, i, mWritePos, mLastFeedbackGain, feedback, false);
    }
    mLastFeedbackGain = feedback;
    
    // advance positions
    mWritePos += buffer.getNumSamples();
    if (mWritePos >= mDelayBuffer.getNumSamples())
        mWritePos -= mDelayBuffer.getNumSamples();
    
    mExpectedReadPos = readPos + buffer.getNumSamples();
    if (mExpectedReadPos >= mDelayBuffer.getNumSamples())
        mExpectedReadPos -= mDelayBuffer.getNumSamples();

}

void Delay::writeToDelayBuffer (AudioBuffer<float> &buffer,
                                const int channelIn, const int channelOut,
                                const int writePos,
                                float startGain, float endGain,
                                bool replacing)
{
        if (writePos + buffer.getNumSamples() <= mDelayBuffer.getNumSamples())
    {
        if (replacing)
            mDelayBuffer.copyFromWithRamp (channelOut, writePos, buffer.getReadPointer (channelIn), buffer.getNumSamples(), startGain, endGain);
        else
            mDelayBuffer.addFromWithRamp (channelOut, writePos, buffer.getReadPointer (channelIn), buffer.getNumSamples(), startGain, endGain);
    }
    else
    {
        const auto midPos  = mDelayBuffer.getNumSamples() - writePos;
        const auto midGain = jmap (float (midPos) / buffer.getNumSamples(), startGain, endGain);
        if (replacing)
        {
            mDelayBuffer.copyFromWithRamp (channelOut, writePos, buffer.getReadPointer (channelIn),         midPos, mLastInputGain, midGain);
            mDelayBuffer.copyFromWithRamp (channelOut, 0,        buffer.getReadPointer (channelIn, midPos), buffer.getNumSamples() - midPos, midGain, endGain);
        }
        else
        {
            mDelayBuffer.addFromWithRamp (channelOut, writePos, buffer.getReadPointer (channelIn),         midPos, mLastInputGain, midGain);
            mDelayBuffer.addFromWithRamp (channelOut, 0,        buffer.getReadPointer (channelIn, midPos), buffer.getNumSamples() - midPos, midGain, endGain);
        }
    }

}

void Delay::readFromDelayBuffer (AudioBuffer<float>& buffer, const int channelIn, const int channelOut, const int readPos, float startGain, float endGain, bool replacing)
{
    if (readPos + buffer.getNumSamples() <= mDelayBuffer.getNumSamples())
    {
        if (replacing)
            buffer.copyFromWithRamp (channelOut, 0, mDelayBuffer.getReadPointer (channelIn, readPos), buffer.getNumSamples(), startGain, endGain);
        else
            buffer.addFromWithRamp (channelOut, 0, mDelayBuffer.getReadPointer (channelIn, readPos), buffer.getNumSamples(), startGain, endGain);
    }
    else
    {
        const auto midPos  = mDelayBuffer.getNumSamples() - readPos;
        const auto midGain = jmap (float (midPos) / buffer.getNumSamples(), startGain, endGain);
        if (replacing)
        {
            buffer.copyFromWithRamp (channelOut, 0,      mDelayBuffer.getReadPointer (channelIn, readPos), midPos, startGain, midGain);
            buffer.copyFromWithRamp (channelOut, midPos, mDelayBuffer.getReadPointer (channelIn), buffer.getNumSamples() - midPos, midGain, endGain);
        }
        else
        {
            buffer.addFromWithRamp (channelOut, 0,      mDelayBuffer.getReadPointer (channelIn, readPos), midPos, startGain, midGain);
            buffer.addFromWithRamp (channelOut, midPos, mDelayBuffer.getReadPointer (channelIn), buffer.getNumSamples() - midPos, midGain, endGain);
        }
    }
}

