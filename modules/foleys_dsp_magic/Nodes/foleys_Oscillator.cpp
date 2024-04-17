//
// Created by Daniel Walz on 25.02.24.
//

#include "foleys_Oscillator.h"

namespace foleys::dsp
{

Oscillator::Oscillator (DspProgram& program, const juce::ValueTree& config) : DspNode (program, config) { }

void Oscillator::prepare (juce::dsp::ProcessSpec spec)
{
    m_oscillator.initialise ([] (float t) { return std::sin (t); }, 1024);
    m_oscillator.setFrequency (440.0f);
    m_oscillator.prepare (spec);
}

void Oscillator::process (juce::dsp::AudioBlock<float>& buffer, juce::MidiBuffer& midi)
{
    buffer.clear();
    m_oscillator.process (juce::dsp::ProcessContextReplacing (buffer));
}

void Oscillator::release() { }


}  // namespace foleys::dsp
