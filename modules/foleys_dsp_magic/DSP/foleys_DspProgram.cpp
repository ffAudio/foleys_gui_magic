//
// Created by Daniel Walz on 04.02.24.
//

#include "foleys_DspProgram.h"


namespace foleys::dsp
{

DspProgram::DspProgram (MagicDspBuilder& builder, const juce::ValueTree& tree)
{
    for (auto node: tree)
    {
        auto child = builder.createNode (node);
        if (child)
        {
            nodeLookup[child->getUID()] = child.get();
            nodes.push_back (std::move (child));
        }
    }

    for (const auto& node: nodes)
    {
        if (auto* input = dynamic_cast<AudioInput*> (node.get()))
            audioInputs.emplace_back (input);
        else if (auto* output = dynamic_cast<AudioOutput*> (node.get()))
            audioOutputs.emplace_back (output);
        else if (auto* minput = dynamic_cast<MidiInput*> (node.get()))
            midiInput = minput;
        else if (auto* moutput = dynamic_cast<MidiOutput*> (node.get()))
            midiOutput = moutput;
    }
}

void DspProgram::prepareToPlay (double sampleRate, int expectedNumSamples)
{
    juce::dsp::ProcessSpec spec { sampleRate, static_cast<juce::uint32> (expectedNumSamples), 2 };

    for (const auto& node: nodes)
        node->prepare (spec);
}

void DspProgram::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midi) { }

void DspProgram::releaseResources()
{
    for (const auto& node: nodes)
        node->release();
}


}  // namespace foleys::dsp
