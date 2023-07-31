
#include "PluginProcessor.h"

VanillaAudioProcessor::VanillaAudioProcessor()
  : foleys::MagicProcessor (
    juce::AudioProcessor::BusesProperties().withInput ("Intput", juce::AudioChannelSet::stereo(), true).withOutput ("Output", juce::AudioChannelSet::stereo(), true))
{
}

void VanillaAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer&)
{
    for (int channel = getTotalNumInputChannels(); channel < getTotalNumOutputChannels(); ++channel)
        buffer.clear (channel, 0, buffer.getNumChannels());

    buffer.clear();
}

bool VanillaAudioProcessor::isBusesLayoutSupported (const juce::AudioProcessor::BusesLayout& layout) const
{
    return (layout.getMainInputChannelSet() == layout.getMainOutputChannelSet());
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new VanillaAudioProcessor();
}
