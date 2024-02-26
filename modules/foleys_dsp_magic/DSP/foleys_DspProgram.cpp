//
// Created by Daniel Walz on 04.02.24.
//

#include "foleys_DspProgram.h"


namespace foleys::dsp
{

DspProgram::DspProgram (MagicDspBuilder& builder, const juce::ValueTree& tree) { }

void DspProgram::prepareToPlay (double sampleRate, int expectedNumSamples)
{
    m_oscillator.initialise ([] (float t) { return std::sin (t); }, 1024);
    m_oscillator.setFrequency (440.0f);
    juce::dsp::ProcessSpec spec;
    spec.sampleRate       = sampleRate;
    spec.maximumBlockSize = expectedNumSamples;
    spec.numChannels      = 2;
    m_oscillator.prepare (spec);
}

void DspProgram::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midi)
{
    buffer.clear();
    juce::dsp::AudioBlock<float> block (buffer);
    m_oscillator.process (juce::dsp::ProcessContextReplacing (block));
}

void DspProgram::releaseResources() { }


}  // namespace foleys::dsp
