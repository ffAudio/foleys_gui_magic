//
// Created by Daniel Walz on 21.04.24.
//

#include "foleys_Biquad.h"

namespace foleys::dsp
{

Biquad::Biquad (DspProgram& program, const juce::ValueTree& config) : DspNode (program, config) { }

void Biquad::prepare (juce::dsp::ProcessSpec spec) { }

void Biquad::process (juce::dsp::AudioBlock<float>& buffer, juce::MidiBuffer& midi) { }

void Biquad::release() { }

int Biquad::getNumParameterInputs() const
{
    return 4;
}

juce::String Biquad::getParameterInputName (int index) const
{
    switch (index)
    {
        case 0: return TRANS ("Filter Type");
        case 1: return TRANS ("Frequency");
        case 2: return TRANS ("Gain");
        case 3: return TRANS ("Quality");
        default: return DspNode::getParameterInputName (index);
    }
}


}  // namespace foleys::dsp
