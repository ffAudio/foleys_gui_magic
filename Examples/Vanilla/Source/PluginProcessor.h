
#pragma once

#include <foleys_gui_magic/foleys_gui_magic.h>

class VanillaAudioProcessor : public foleys::MagicProcessor
{
public:
    VanillaAudioProcessor();

    void prepareToPlay ([[maybe_unused]] double sampleRate, [[maybe_unused]] int expectedNumSamples) override { }
    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;
    void releaseResources() override { }

    bool isBusesLayoutSupported (const juce::AudioProcessor::BusesLayout& layout) const override;

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (VanillaAudioProcessor)
};
