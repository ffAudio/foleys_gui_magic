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
class ExtendingExampleAudioProcessor  : public foleys::MagicProcessor
{
public:
    //==============================================================================
    ExtendingExampleAudioProcessor();
    ~ExtendingExampleAudioProcessor() override;

    //==============================================================================
    void initialiseBuilder (foleys::MagicGUIBuilder& builder) override;

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const juce::AudioProcessor::BusesLayout& layouts) const override;
   #endif

    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    //==============================================================================
    const juce::String getName() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    std::atomic<double> statisticsSampleRate;
    std::atomic<int>    statisticsSamplesPerBlock;

private:
    //==============================================================================

    juce::AudioProcessorValueTreeState treeState { *this, nullptr, "PARAMETERS", juce::AudioProcessorValueTreeState::ParameterLayout() };

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ExtendingExampleAudioProcessor)
};
