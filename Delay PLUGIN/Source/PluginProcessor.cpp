/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

DelayPluginAudioProcessor::DelayPluginAudioProcessor() : parameters(*this, nullptr, "PARAMETERS", createLayout()),
#ifndef JucePlugin_PreferredChannelConfigurations
      AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", AudioChannelSet::stereo(), true)
                     #endif
                       )
#endif
{
    
}

DelayPluginAudioProcessor::~DelayPluginAudioProcessor()
{
}

////////////////////////////////////////////////////////////////////////////////
//////////////////// NO NEED TO TOUCH ANYTHING IN HERE /////////////////////////

const String DelayPluginAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool DelayPluginAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool DelayPluginAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool DelayPluginAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double DelayPluginAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int DelayPluginAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int DelayPluginAudioProcessor::getCurrentProgram()
{
    return 0;
}

void DelayPluginAudioProcessor::setCurrentProgram (int index)
{
}

const String DelayPluginAudioProcessor::getProgramName (int index)
{
    return {};
}

void DelayPluginAudioProcessor::changeProgramName (int index, const String& newName)
{
}


///////////////////////////////////////////////////////////////////////////////////


void DelayPluginAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    // Use this method as the place to do any pre-playback
    // initialisation that you need..
    
    lastSampleRate = sampleRate;
	delayBuffer.setSize(2, sampleRate*5);
	delayBuffer.clear();
}

void DelayPluginAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool DelayPluginAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    if (layouts.getMainOutputChannelSet() != AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}
#endif

void DelayPluginAudioProcessor::processBlock (AudioBuffer<float>& buffer, MidiBuffer& midiMessages)
{
    ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    // In case we have more outputs than inputs, this code clears any output
    // channels that didn't contain input data, (because these aren't
    // guaranteed to be empty - they may contain garbage).
    // This is here to avoid people getting screaming feedback
    // when they first compile a plugin, but obviously you don't need to keep
    // this code if your algorithm always overwrites all the output channels.
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());

    // This is the place where you'd normally do the guts of your plugin's
    // audio processing...
    // Make sure to reset the state if your inner loop is processing
    // the samples and the outer loop is handling the channels.
    // Alternatively, you can process the samples with the channels
    // interleaved by keeping the same state.
    
    
    //clearActiveBufferRegion(); //erases all input as it comes in DONT WANT!

	int numSamples = buffer.getNumSamples();
	int delaySamples = delayBuffer.getNumSamples();

	for (int i = 0; i < buffer.getNumChannels(); i++) {
		fillDelayBuffer(buffer, i);
		readFromDelayBuffer(buffer, i);
	}

	writePosition += numSamples;
	writePosition %= delaySamples;

//		combine noise with the signal
//		for (int i = 0; i < buffer.getNumSamples(); i++)
//		{
//			buffer[i] *= Random().nextFloat();
//		}

}

bool DelayPluginAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

AudioProcessorEditor* DelayPluginAudioProcessor::createEditor()
{
    return new DelayPluginAudioProcessorEditor (*this);
}

void DelayPluginAudioProcessor::getStateInformation (MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void DelayPluginAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}


///////////////////////////////////////////////////////////////////////////////////


void DelayPluginAudioProcessor::fillDelayBuffer(AudioBuffer<float> &buffer, int channel)
{
	int numSamples = buffer.getNumSamples();
	int delaySamples = delayBuffer.getNumSamples();

	if (writePosition + numSamples >= delaySamples)
	{
		int difference = delaySamples - writePosition;
		int leftOverSamples = numSamples - difference;

		//check if you are at the end of buffer and have to wrap around and write from beginning
		delayBuffer.copyFrom(channel, writePosition, buffer, channel, 0, difference);
		delayBuffer.copyFrom(channel, 0, buffer, channel, difference, leftOverSamples);
	}
	else {
		delayBuffer.copyFrom(channel, writePosition, buffer, channel, 0, numSamples);
	}
}

void DelayPluginAudioProcessor::readFromDelayBuffer(AudioBuffer<float>& buffer, int channel)
{
	int numSamples = buffer.getNumSamples();
	int delaySamples = delayBuffer.getNumSamples();
	float delayTime = *parameters.getRawParameterValue("delayTime");
	DBG(delayTime);

	//wrap around from end of last buffer to start of next one
	//delayTime is ms and fs is in seconds -> math to compensate
	//mod by delaybuffer length to wrap around when near the end of buffer
	int readPosition = static_cast<int>(delaySamples + writePosition - (lastSampleRate * delayTime / 1000) ) % delaySamples;

	if (readPosition + numSamples >= delaySamples) {	//if you exceed length of delay buffer
		int difference = delaySamples - readPosition;	//number of samples left until the end of delaybuffer
		int leftOverSamples = numSamples - difference;	//number of samples that get cut off and have to be put at the beginning

		buffer.addFrom(channel, 0, delayBuffer, channel, readPosition, difference); //from readPosition to end of buffer
		buffer.addFrom(channel, difference, delayBuffer, channel, 0, leftOverSamples); //from 0 to left over amount
	}
	else { //if you do NOT have to wrap around to the beginning, read from delay buffer normally
		buffer.addFrom(channel, 0, delayBuffer, channel, readPosition, numSamples);
	}

}


//look up the tutorial for Audio Tree State
AudioProcessorValueTreeState::ParameterLayout DelayPluginAudioProcessor::createLayout() {
	AudioProcessorValueTreeState::ParameterLayout layout;
	layout.add( std::make_unique<AudioParameterFloat>( "delayTime", "Delay Time (ms)", NormalisableRange<float>(0.0, 2000.0), 500.0 ) );
	return layout;
}


//==============================================================================
// This creates new instances of the plugin..
AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new DelayPluginAudioProcessor();
}
