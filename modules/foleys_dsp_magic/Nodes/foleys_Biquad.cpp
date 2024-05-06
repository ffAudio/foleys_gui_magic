//
// Created by Daniel Walz on 21.04.24.
//

#include "foleys_Biquad.h"

namespace foleys::dsp
{

Biquad::Biquad (DspProgram& program, const juce::ValueTree& config) : DspNode (program, config)
{
    addAudioInput (TRANS ("Audio In"));
    addAudioOutput (TRANS ("Audio Out"));

    addParameterInput (TRANS ("Filter Type"));
    addParameterInput (TRANS ("Frequency"));
    addParameterInput (TRANS ("Gain"));
    addParameterInput (TRANS ("Quality"));
}

void Biquad::prepare ([[maybe_unused]] juce::dsp::ProcessSpec spec) { }

void Biquad::process ([[maybe_unused]] int numSamples) { }

}  // namespace foleys::dsp
