/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/


/*
- future possible additions: hover over some point in the signal and give user more info (such as amplitude in the osc at a point or effect info)
- future possible: zoom into see more accurate delay time
feedback: use bar graphs to represent gain of delayed signal OR hover text for each knob (not obligated to read)

- references: isotope! ableton too much visual stimulation...? 
- kind of foundational paper in interface design -> Interactive User Interface Design by IEEE to site!
- Edward Tufte da
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
                       ), lfo(*this) // "this" is the reference to the processor bc WE ARE IN the processor rn
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

	lastDelayTime = *parameters.getRawParameterValue("delayTime");
	smoothedValue = SmoothedValue<float, ValueSmoothingTypes::Linear>(lastDelayTime); //initial value of current delayTime

	lfo.prepareToPlay(sampleRate);
}

////////////////////////////////////////////////////////////////////////////////
//////////////////// NO NEED TO TOUCH ANYTHING IN HERE /////////////////////////

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

///////////////////////////////////////////////////////////////////////////////////

void DelayPluginAudioProcessor::processBlock (AudioBuffer<float>& buffer, MidiBuffer& midiMessages)
{
    ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    // In case we have more outputs than inputs, this code clears any output
    // channels that didn't contain input data, (because these aren't
    // guaranteed to be empty - they may contain garbage).
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());

    // This is the place where you'd normally do the guts of your plugin's
    // audio processing...
    // Make sure to reset the state if your inner loop is processing
    // the samples and the outer loop is handling the channels.
    // Alternatively, you can process the samples with the channels
    // interleaved by keeping the same state.
    

	int numSamples = buffer.getNumSamples();
	int delaySamples = delayBuffer.getNumSamples();
	int currentDelayMode = int(*parameters.getRawParameterValue("delayMode"));

	//see which delay mode we're in and set currentDelayTime accordingly
	//manual = get slider value from delayTime parameter
	if (currentDelayMode == DelayMode::manualMode) {
		currentDelayTime = *parameters.getRawParameterValue("delayTime");
	}

	//lfo modulated = create and write to LFO buffer, get RMS level (current amplitude), and scale that and set it as currentDelayTime
	else if (currentDelayMode == DelayMode::lfoMode) {
		AudioBuffer<float> lfoBuffer; //make a buffer to store your lfo signal values
		lfoBuffer.setSize(1, buffer.getNumSamples()); //make the size that of the output buffer
		lfo.processBlock(lfoBuffer); //write to the lfo buffer (NOT the output buffer)
		float currentLFOAmplitude = 0;
		currentLFOAmplitude = lfoBuffer.getRMSLevel(0, 0, buffer.getNumSamples()); //get the rms (amplitude) of the lfo signal

		currentDelayTime = ceil(currentLFOAmplitude * 1000);
	}

	//amplitude modulates = get RMS level (amplitude) of the input signal (the buffer), scale it and set it as currentDelayTime
	else if (currentDelayMode == DelayMode::amplitudeMode) {
		float currentInputAmplitude = 0;
		currentInputAmplitude = buffer.getRMSLevel(0, 0, buffer.getNumSamples());
		currentDelayTime = ceil(currentInputAmplitude * 1000);
	}

	else {}

	for (int i = 0; i < buffer.getNumChannels(); i++) {
		float* drySignalBuffer = buffer.getWritePointer(i); //buffer to add the main signal to for feedback

		fillDelayBuffer(buffer, i);
		readFromDelayBuffer(buffer, i);
		feedback(buffer, i, drySignalBuffer);
	}

	writePosition += numSamples;
	writePosition %= delaySamples;

//		how to combine noise with the signal
//		for (int i = 0; i < buffer.getNumSamples(); i++)
//		{
//			buffer[i] *= Random().nextFloat();
//		}

}


////////////////////////////////////////////////////////////////////////////////
//////////////////// NO NEED TO TOUCH ANYTHING IN HERE /////////////////////////


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
	
	//wrap around from end of last buffer to start of next one
	//delayTime is ms and fs is in seconds -> math to compensate
	//mod by delaybuffer length to wrap around when near the end of buffer
	int readPosition = static_cast<int>(delaySamples + writePosition - (lastSampleRate * currentDelayTime/1000) ) % delaySamples;
	
	if (lastDelayTime != currentDelayTime) { //if you turn the knob
		if (smoothedValue.getTargetValue() != currentDelayTime) { //if you keep turning the knob
			smoothedValue.setTargetValue(currentDelayTime); //update the target value
			lastDelayTime = smoothedValue.getNextValue(); //do the smoothing
		}
	}

	if (readPosition + numSamples >= delaySamples) {	//if you exceed length of delay buffer
		int difference = delaySamples - readPosition;	//number of samples left until the end of delaybuffer
		int leftOverSamples = numSamples - difference;	//number of samples that get cut off and have to be put at the beginning

		buffer.copyFrom(channel, 0, delayBuffer, channel, readPosition, difference); //from readPosition to end of buffer
		buffer.copyFrom(channel, difference, delayBuffer, channel, 0, leftOverSamples); //from 0 to left over amount
	}
	else { //if you do NOT have to wrap around to the beginning, read from delay buffer normally
		buffer.copyFrom(channel, 0, delayBuffer, channel, readPosition, numSamples);
	}

}

void DelayPluginAudioProcessor::feedback(AudioBuffer<float>& buffer, int channel, float* drySignalBuffer) {
	int numSamples = buffer.getNumSamples();
	int delaySamples = delayBuffer.getNumSamples();
	float gain = 0.7;
	gain = *parameters.getRawParameterValue("feedback");
	//DBG(gain);

	if (numSamples + writePosition >= delaySamples) {
		int difference = delaySamples - writePosition;
		int leftOverSamples = numSamples - difference;

		delayBuffer.addFromWithRamp(channel, difference, drySignalBuffer, difference, gain, gain);
		delayBuffer.addFromWithRamp(channel, 0, drySignalBuffer, leftOverSamples, gain, gain);
	}
	else {
		delayBuffer.addFromWithRamp(channel, writePosition, drySignalBuffer, numSamples, gain, gain);
	}
}


//look up the tutorial for Audio Tree State
//these are the parameters that the user can control in the interface
AudioProcessorValueTreeState::ParameterLayout DelayPluginAudioProcessor::createLayout() {
	AudioProcessorValueTreeState::ParameterLayout layout;
	layout.add( std::make_unique<AudioParameterFloat>( "delayTime", "Delay Time (ms)", NormalisableRange<float>(0.0, 2000.0), 500.0 ) );
	layout.add( std::make_unique<AudioParameterFloat>( "feedback", "Feedback", NormalisableRange<float>(0.0, 1.0), 0.0) );
	layout.add( std::make_unique<AudioParameterFloat>( "lfoFrequency", "LFO Frequency", NormalisableRange<float>(0.1, 4.0), 1.0) );

	//this parameter is for a 3-way toggle switch that allows the user to choose from manual delay time (the knob), lfo modulated delay time, or input signal amplitude modulated delay time
	layout.add(std::make_unique<AudioParameterInt>("delayMode", "Delay Mode", 1, 3, 0));

	return layout;
}


//==============================================================================
// This creates new instances of the plugin..
AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new DelayPluginAudioProcessor();
}
