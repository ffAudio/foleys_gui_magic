/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"

//==============================================================================
/**
*/
class SignalGeneratorAudioProcessor  : public  foleys::MagicProcessor,
                                       private juce::AudioProcessorValueTreeState::Listener
{
public:
    enum WaveType
    {
        None = 0,
        Sine,
        Triangle,
        Square
    };

    //==============================================================================
    SignalGeneratorAudioProcessor();
    ~SignalGeneratorAudioProcessor() override;

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const juce::AudioProcessor::BusesLayout& layouts) const override;
   #endif

    void parameterChanged (const juce::String& param, float value) override;

    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    //==============================================================================
    double getTailLengthSeconds() const override;

private:
    //==============================================================================

    void setOscillator (juce::dsp::Oscillator<float>& osc, WaveType type);

    std::atomic<float>* frequency  = nullptr;
    std::atomic<float>* level      = nullptr;

    std::atomic<float>* lfoFrequency  = nullptr;
    std::atomic<float>* lfoLevel      = nullptr;

    std::atomic<float>* vfoFrequency  = nullptr;
    std::atomic<float>* vfoLevel      = nullptr;

    juce::dsp::Oscillator<float> mainOSC;
    juce::dsp::Oscillator<float> lfoOSC;
    juce::dsp::Oscillator<float> vfoOSC;

    juce::AudioProcessorValueTreeState treeState;

    // MAGIC GUI: this is a shorthand where the samples to display are fed to
    foleys::MagicPlotSource*    oscilloscope = nullptr;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SignalGeneratorAudioProcessor)
};
