/*
  ==============================================================================

    FoleysSynth.h
    Created: 23 Nov 2019 8:08:35pm
    Author:  Daniel Walz

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"

class FoleysSynth : public juce::Synthesiser
{
public:
    static int  numOscillators;

    static void addADSRParameters (juce::AudioProcessorValueTreeState::ParameterLayout& layout);
    static void addOvertoneParameters (juce::AudioProcessorValueTreeState::ParameterLayout& layout);
    static void addGainParameters (juce::AudioProcessorValueTreeState::ParameterLayout& layout);

    FoleysSynth() = default;

    class FoleysSound : public juce::SynthesiserSound
    {
    public:
        FoleysSound (juce::AudioProcessorValueTreeState& state);
        bool appliesToNote (int) override { return true; }
        bool appliesToChannel (int) override { return true; }

        juce::ADSR::Parameters getADSR();

    private:
        juce::AudioProcessorValueTreeState& state;
        juce::AudioParameterFloat* attack  = nullptr;
        juce::AudioParameterFloat* decay   = nullptr;
        juce::AudioParameterFloat* sustain = nullptr;
        juce::AudioParameterFloat* release = nullptr;
        juce::AudioParameterFloat* gain    = nullptr;

        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (FoleysSound)
    };

    class FoleysVoice : public juce::SynthesiserVoice
    {
    public:
        FoleysVoice (juce::AudioProcessorValueTreeState& state);

        bool canPlaySound (juce::SynthesiserSound *) override;

        void startNote (int midiNoteNumber,
                        float velocity,
                        juce::SynthesiserSound* sound,
                        int currentPitchWheelPosition) override;

        void stopNote (float velocity, bool allowTailOff) override;

        void pitchWheelMoved (int newPitchWheelValue) override;

        void controllerMoved (int controllerNumber, int newControllerValue) override;

        void renderNextBlock (juce::AudioBuffer<float>& outputBuffer,
                              int startSample,
                              int numSamples) override;

        void setCurrentPlaybackSampleRate (double newRate) override;

    private:

        class BaseOscillator
        {
        public:
            BaseOscillator() = default;

            juce::dsp::ProcessorChain<juce::dsp::Oscillator<float>, juce::dsp::Gain<float>> osc;
            juce::AudioParameterFloat* gain   = nullptr;
            juce::AudioParameterFloat* detune = nullptr;
            double multiplier = 1.0;

        private:
            JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (BaseOscillator)
        };

        double getDetuneFromPitchWheel (int wheelValue) const;
        double getFrequencyForNote (int noteNumber, double detune, double concertPitch = 440.0) const;

        void updateFrequency (BaseOscillator& oscillator, bool noteStart = false);

        std::vector<std::unique_ptr<BaseOscillator>> oscillators;

        double                      pitchWheelValue = 0.0;
        int                         maxPitchWheelSemitones = 12;
        const int                   internalBufferSize = 64;
        juce::AudioBuffer<float>    oscillatorBuffer;
        juce::AudioBuffer<float>    voiceBuffer;
        juce::ADSR                  adsr;
        juce::AudioParameterFloat*  gainParameter = nullptr;
        float                       lastGain = 0.0;

        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (FoleysVoice)
    };

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (FoleysSynth)
};
