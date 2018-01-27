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
		mySynth1.addVoice(new OscillatorVoice(OscillatorType::sineWave));
		mySynth1.addSound(new SynthSound());
		
		mySynth2.clearSounds();
		mySynth2.addVoice(new OscillatorVoice(OscillatorType::squareWave));
		mySynth2.addSound(new SynthSound());
	}

	~SynthProcessor() {	}

	// TODO: simplify this function
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

		addEffects(buffer);
	}

	void addEffects(AudioBuffer<float> &buffer) {
		const int numSamples = buffer.getNumSamples();
		const int numChannels = buffer.getNumChannels();

		for (int sample = 0; sample < numSamples; ++sample) {

			for (int channel = 0; channel < numChannels; ++channel) {

				// Apply distortion effect
				float sampleToAdd = distortion.atanDist(buffer.getSample(channel, sample), 15) * gainParam->get();

				buffer.addSample(channel, sample, sampleToAdd);
			}
		}
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

	//derp
	const String getProgramName(int index) override {
		return "The Snyth";
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
