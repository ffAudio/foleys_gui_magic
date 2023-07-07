/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "FoleysSynth.h"

class PresetListBox;

//==============================================================================
/**
*/
class FoleysSynthAudioProcessor  : public foleys::MagicProcessor
{
public:
    //==============================================================================
    FoleysSynthAudioProcessor();

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const juce::AudioProcessor::BusesLayout& layouts) const override;
   #endif

    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    //==============================================================================

    //==============================================================================
    void savePresetInternal();
    void loadPresetInternal(int index);

    //==============================================================================
    double getTailLengthSeconds() const override;

private:
    //==============================================================================
    juce::AudioProcessorValueTreeState treeState;

    FoleysSynth      synthesiser;
    juce::ValueTree  presetNode;

    // GUI MAGIC: define that as last member of your AudioProcessor
    foleys::MagicLevelSource*   outputMeter  = nullptr;
    foleys::MagicPlotSource*    oscilloscope = nullptr;
    foleys::MagicPlotSource*    analyser     = nullptr;

    PresetListBox*              presetList   = nullptr;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (FoleysSynthAudioProcessor)
};
