/*
  ==============================================================================

    This file was auto-generated!

  ==============================================================================
*/

#include "MainComponent.h"
#include <stdlib.h>


MainComponent::MainComponent():deviceSelector(deviceManager, 1, 2, 1, 2, false, false, false, true)
	//using deviceSelector constructor on that line bc cant create it after MainComponent has been called, if you had a pointer then you can create it after
{
    // Make sure you set the size of the component after
    // you add any child components.
	addAndMakeVisible(deviceSelector);

	addAndMakeVisible(button);
	button.setButtonText("BUTTON");

	button.onClick = [&] {

		//write to an output file
		//File f = File::getSpecialLocation(File::SpecialLocationType::userDesktopDirectory);
		File f("C:/jucing/output.wav");
		WavAudioFormat format;

		std::unique_ptr<AudioFormatWriter> writer;
		writer.reset(format.createWriterFor(new FileOutputStream(f), lastSampleRate, 1, 24, "", 0));
		if (writer != nullptr) {
			writer->writeFromAudioSampleBuffer(delayBuffer, 0, delayBuffer.getNumSamples());
		}

	};

    setSize(800, 600);

    // Some platforms require permissions to open input channels so request that here
    if (RuntimePermissions::isRequired (RuntimePermissions::recordAudio)
        && ! RuntimePermissions::isGranted (RuntimePermissions::recordAudio))
    {
        RuntimePermissions::request (RuntimePermissions::recordAudio,
                                     [&] (bool granted) { if (granted)  setAudioChannels (2, 2); });
    }
    else
    {
        // Specify the number of input and output channels that we want to open
        setAudioChannels (2, 2);
    }
}

MainComponent::~MainComponent()
{
    // This shuts down the audio device and clears the audio source.
    shutdownAudio();
}


///////////////////////////////////////////////////////////////////////////////////


void MainComponent::prepareToPlay (int samplesPerBlockExpected, double sampleRate)
{
    // This function will be called when the audio device is started, or when
    // its settings (i.e. sample rate, block size, etc) are changed.

    // You can use this function to initialise any resources you might need,
    // but be careful - it will be called on the audio thread, not the GUI thread.

    // For more details, see the help for AudioProcessor::prepareToPlay()

	lastSampleRate = sampleRate;
	delayBuffer.setSize(2, sampleRate*5);
	delayBuffer.clear();
}

void MainComponent::getNextAudioBlock (const AudioSourceChannelInfo& bufferToFill)
{
    // Your audio-processing code goes here!

    // For more details, see the help for AudioProcessor::getNextAudioBlock()

    // Right now we are not producing any data, in which case we need to clear the buffer
    // (to prevent the output of random noise)

    //bufferToFill.clearActiveBufferRegion(); //erases all input as it comes in DONT WANT!

	int numSamples = bufferToFill.buffer->getNumSamples();
	int delaySamples = delayBuffer.getNumSamples();

	for (int i = 0; i < bufferToFill.buffer->getNumChannels(); i++) {
		fillDelayBuffer(*bufferToFill.buffer, i);
		readFromDelayBuffer(*bufferToFill.buffer, i);
	}

	writePosition += numSamples;
	writePosition %= delaySamples;

//		combine noise with the signal
//		for (int i = 0; i < bufferToFill.buffer->getNumSamples(); i++)
//		{
//			buffer[i] *= Random().nextFloat();
//		}

}

void MainComponent::releaseResources()
{
    // This will be called when the audio device stops, or when it is being
    // restarted due to a setting change.

    // For more details, see the help for AudioProcessor::releaseResources()
}

void MainComponent::paint (Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (ResizableWindow::backgroundColourId));

    // You can add your drawing code here!
}

void MainComponent::resized()
{
    // This is called when the MainContentComponent is resized.
    // If you add any child components, this is where you should
    // update their positions.

	Rectangle<int> area = getLocalBounds();
	area = area.removeFromTop(getHeight() / 2);

	deviceSelector.setBounds(area);
	area.translate(0, getHeight() / 2);
	button.setBounds(area);
}


///////////////////////////////////////////////////////////////////////////////////


void MainComponent::fillDelayBuffer(AudioBuffer<float> &buffer, int channel)
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

void MainComponent::readFromDelayBuffer(AudioBuffer<float>& buffer, int channel)
{
	int numSamples = buffer.getNumSamples();
	int delaySamples = delayBuffer.getNumSamples();
	int delayTime = 1000; //time (ms)

	//wrap around from end of last buffer to start of next one
	//delayTime is ms and fs is in seconds -> math to compensate
	//mod by delaybuffer length to wrap around when near the end of buffer
	int readPosition = static_cast<int>(delaySamples + writePosition - (lastSampleRate * delayTime / 1000) ) % delaySamples;

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