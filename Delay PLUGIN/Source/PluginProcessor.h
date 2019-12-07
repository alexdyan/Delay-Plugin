/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "LookAndFeelHolder.h"
#include "DelayPluginLookAndFeel.h"
#include "LFO.h"

/**
*/
class DelayPluginAudioProcessor  : public AudioProcessor
{
public:
	enum DelayMode:int {
		manualMode = 1,
		lfoMode,
		amplitudeMode
	};

    DelayPluginAudioProcessor();
    ~DelayPluginAudioProcessor();

    ////////////////////////////////////////////////////////////////////////////////

    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

    void processBlock (AudioBuffer<float>&, MidiBuffer&) override;

    ////////////////////////////////////////////////////////////////////////////////

    AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    ////////////////////////////////////////////////////////////////////////////////

    const String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    ////////////////////////////////////////////////////////////////////////////////

    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const String getProgramName (int index) override;
    void changeProgramName (int index, const String& newName) override;

    ////////////////////////////////////////////////////////////////////////////////

    void getStateInformation (MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

    ////////////////////////////////////////////////////////////////////////////////

	AudioProcessorValueTreeState parameters;
	float currentDelayTime = 0;

private:
    void fillDelayBuffer(AudioBuffer<float>& buffer, int channel, int writePos, float startGain, float endGain, bool replacing);
	void readFromDelayBuffer(AudioBuffer<float> &buffer, int channel, int readPos, float startGain, float endGain, bool replacing);
	void feedback(AudioBuffer<float>& buffer, int channel, float* drySignalBuffer);
	AudioProcessorValueTreeState::ParameterLayout createLayout();


	double lastSampleRate;
	float lastDelayTime; //need this for smoothing the delay time
	int writePosition = 0;
	int nextReadPos = -1;
	float lastGain = 0;
	float lastFeedbackGain = 0;

	AudioBuffer<float> delayBuffer;
	SmoothedValue<float, ValueSmoothingTypes::Linear> smoothedValue;
	LookAndFeelHolder<DelayPluginLookAndFeel> lookAndFeel;
	LFO lfo;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (DelayPluginAudioProcessor)
};
