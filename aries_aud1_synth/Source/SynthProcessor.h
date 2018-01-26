/*
  ==============================================================================

    SynthProcessor.h
    Created: 21 Jan 2018 9:55:11pm
    Author:  Chris.Buttacavoli

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "SynthSound.h"
#include "OscillatorVoice.h"
#include "SynthAudioProcessEditor.h"
#include "maximilian.h"


//==============================================================================
/*
*/
class SynthProcessor : public AudioProcessor
{
public:
	SynthProcessor(MidiKeyboardState &keyState) : keyboardState(keyState) {

		//initializing gain param
		addParameter(gainParam = new AudioParameterFloat("gain", "Gain", 0.0f, 1.0f, 0.9f));
		
		mySynth1.clearSounds();
		mySynth2.clearSounds();

		for (int i = 0; i < 5; i++)
		{
			mySynth1.addVoice(new OscillatorVoice(OscillatorType::sineWave));
			mySynth2.addVoice(new OscillatorVoice(OscillatorType::squareWave));
		}
		mySynth1.addSound(new SynthSound());
		mySynth2.addSound(new SynthSound());

		this->initEnvelope();
	}

	~SynthProcessor() {	}

	const String getName() const override {
		return String("My Synth Processor");
	}

	void prepareToPlay(double sampleRate, int MaxSamplesPerBlock) override{

		midiCollector.reset(sampleRate);

		mySynth1.setCurrentPlaybackSampleRate(sampleRate);
		mySynth2.setCurrentPlaybackSampleRate(sampleRate);
	}
	
	void processBlock(AudioBuffer<float> &buffer, MidiBuffer &midiMessages) override {
		// the synth always adds its output to the audio buffer, so we have to clear it
		// first..
		buffer.clear();

		// fill a midi buffer with incoming messages from the midi input.
		MidiBuffer incomingMidi;
		midiCollector.removeNextBlockOfMessages(incomingMidi, buffer.getNumSamples());

		// pass these messages to the keyboard state so that it can update the component
		// to show on-screen which keys are being pressed on the physical midi keyboard.
		// This call will also add midi messages to the buffer which were generated by
		// the mouse-clicking on the on-screen keyboard.
		keyboardState.processNextMidiBuffer(incomingMidi, 0, buffer.getNumSamples(), true);

		// and now get the synth to process the midi events and generate its output.
		mySynth1.renderNextBlock(buffer, incomingMidi, 0, buffer.getNumSamples());
		//mySynth2.renderNextBlock(buffer, incomingMidi, 0, buffer.getNumSamples());
		//mySynth3.renderNextBlock(buffer, incomingMidi, 0, buffer.getNumSamples());
		//mySynth4.renderNextBlock(buffer, incomingMidi, 0, buffer.getNumSamples());


		//Applying distortion - Victoria
		//Commenting out for now so as not to mess with the env code in the loop below
		/*atan distortion, see http://www.musicdsp.org/showArchiveComment.php?ArchiveID=104*/
		/*shape from 1 (soft clipping) to infinity (hard clipping)*/
		// Always apply effects, control using parameters through GUI
		for (int sample = 0; sample < buffer.getNumSamples(); ++sample) {
			
			for (int channel = 0; channel < buffer.getNumChannels(); ++channel) {
				//float sampleToAdd = buffer.getSample(channel, sample);
				
				// Apply distortion
				//distortion.atanDist(sampleToAdd, 15);

				// Envelope last
				//env.adsr(sampleToAdd, 1);
				buffer.addSample(channel, sample, (distortion.atanDist(buffer.getSample(channel, sample), 15) * gainParam->get()));
				
				// Apply all effects to the sample
				//buffer.addSample(channel, sample, sampleToAdd);
			}
		}
	}

	void initEnvelope() {
		env.setAttack(1);
		env.setDecay(1);
		env.setSustain(100);
		env.setRelease(5000);
		env.amplitude = 1;
		env.trigger = 1;
	}

	void releaseResources() override {

	}

	double getTailLengthSeconds() const override {
		return 0;
	}

	bool acceptsMidi() const override {
		return true;
	}

	bool producesMidi() const override {
		return true;
	}


	/*************
	** Create Process Editor Class to generete new UI elements
	**
	*************/
	AudioProcessorEditor* createEditor() override{
		//not sure what to put here
		return new GenericEditor (*this);
	}

	bool hasEditor() const override {
		return true;
	}

	int getNumPrograms() override {
		//temp?
		return 0;
	}

	void changeProgramName(int index, const String &newName) override {
		
	}

	int getCurrentProgram() override {
		//temp?
		return 0;
	}

	void setCurrentProgram(int index) override {
		
	}

	const String getProgramName(int index) override {
		return "The Synth";
	}

	void setStateInformation(const void * data, int sizeInBytes) override {

	}

	void getStateInformation(juce::MemoryBlock &destData) override {

	}

	//Getting midi/keyboard messages
	MidiMessageCollector midiCollector;
	MidiKeyboardState& keyboardState;

	//the actual synth object
	//the actual synth object
	Synthesiser mySynth1;
	Synthesiser mySynth2;

	// Effects for now
	maxiDistortion distortion;
	maxiEnv env;

	// Our parameters
	AudioParameterFloat* gainParam = nullptr;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SynthProcessor)
};
