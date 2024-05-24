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

    addParameterInput (TRANS ("Type"), 0.0f, 0.0f, 5.0f);
    addParameterInput (TRANS ("Frequency"), 1000.0f, 20.0f, 20000.0f);
    addParameterInput (TRANS ("Gain"), 1.0f, 0.0f, 1.0f);
    addParameterInput (TRANS ("Q"), 1.0f, 0.01f, 10.0f);
}

void Biquad::prepare ([[maybe_unused]] juce::dsp::ProcessSpec spec) { }

void Biquad::process ([[maybe_unused]] int numSamples) { }

}  // namespace foleys::dsp
