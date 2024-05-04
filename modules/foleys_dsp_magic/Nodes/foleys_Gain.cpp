//
// Created by Daniel Walz on 03.05.24.
//

#include "foleys_Gain.h"


namespace foleys::dsp
{

Gain::Gain (DspProgram& program, const juce::ValueTree& config) : DspNode (program, config)
{
    addAudioInput (TRANS ("Audio In"));
    addParameterInput (TRANS ("Gain"));

    addAudioOutput (TRANS ("Audio Out"));
}

void Gain::prepare ([[maybe_unused]] juce::dsp::ProcessSpec spec) { }

void Gain::process()
{
    auto* audioOutput = getOutput (ConnectionType::Audio, 0);
    jassert (audioOutput);

    audioOutput->setAudioBlock ({});

    if (auto* input = getAudioInputs().front().getOutput())
        audioOutput->setAudioBlock (input->getAudio());
    else
        return;

    if (auto* gain = getParameterInputs().front().getOutput())
    {
        gain->multiply (audioOutput->getAudio());
    }
}


}  // namespace foleys::dsp
