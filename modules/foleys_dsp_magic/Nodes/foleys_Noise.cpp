//
// Created by Daniel Walz on 25.02.24.
//

#include "foleys_Noise.h"

namespace foleys::dsp
{

Noise::Noise (DspProgram& program, const juce::ValueTree& config) : DspNode (program, config) { }

void Noise::prepare (juce::dsp::ProcessSpec spec)
{
//    m_oscillator.initialise ([] (float t) { return std::sin (t); }, 1024);
//    m_oscillator.setFrequency (440.0f);
//    m_oscillator.prepare (spec);
}

void Noise::process (juce::dsp::AudioBlock<float>& buffer, juce::MidiBuffer& midi)
{
    buffer.clear();
//    m_oscillator.process (juce::dsp::ProcessContextReplacing (buffer));
}

void Noise::release() { }


}  // namespace foleys::dsp
