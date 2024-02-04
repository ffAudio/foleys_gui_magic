//
// Created by Daniel Walz on 04.02.24.
//

#pragma once

#include "foleys_BuiltinNodes.h"

namespace foleys
{

class DspProgram
{
public:
    explicit DspProgram (const juce::ValueTree& tree);

    void prepareToPlay(double sampleRate, int expectedNumSamples);
    void processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midi);
    void releaseResources();

    bool acceptsMidi() const { return m_midiInput != nullptr; }
    bool producesMidi() const { return m_midiOutput != nullptr; }
    bool isMidiEffect() const { return false; }

private:
    std::unique_ptr<MidiInput>  m_midiInput;
    std::unique_ptr<MidiOutput> m_midiOutput;

    std::vector<std::unique_ptr<AudioInput>> m_audioInputs;
    std::vector<std::unique_ptr<AudioInput>> m_audioOutputs;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (DspProgram)
};


}  // namespace foleys
