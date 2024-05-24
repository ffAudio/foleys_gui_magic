//
// Created by Daniel Walz on 03.05.24.
//

#include "foleys_Gain.h"


namespace foleys::dsp
{

Gain::Gain (DspProgram& program, const juce::ValueTree& config) : DspNode (program, config)
{
    addAudioInput (TRANS ("Audio In"));
    addParameterInput (TRANS ("Gain"), 1.0f, 0.0f, 1.0f);
    addParameterInput (TRANS ("Phase"), 0.0f, 0.0f, 1.0f);

    addAudioOutput (TRANS ("Audio Out"));
}

void Gain::prepare ([[maybe_unused]] juce::dsp::ProcessSpec spec) { }

void Gain::process ([[maybe_unused]] int numSamples)
{
    auto* audioOutput = getOutput (ConnectionType::Audio, 0);
    jassert (audioOutput);

    audioOutput->setAudioBlock ({});

    if (auto* input = getConnectedOutput (ConnectionType::Audio, 0))
        audioOutput->setAudioBlock (input->getAudio());
    else
        return;

    bool phaseSwap = false;
    if (auto* phaseParameter = getConnectedOutput (ConnectionType::Parameter, 1))
        phaseSwap = phaseParameter->getStaticValue() < 0.5f;

    if (auto* gain = getConnectedOutput (ConnectionType::Parameter, 0))
    {
        if (phaseSwap)
            gain->multiply (audioOutput->getAudio(), 0.0f, -1.0f);
        else
            gain->multiply (audioOutput->getAudio(), 0.0f, 1.0f);
    }
    else if (phaseSwap)
    {
        audioOutput->getAudio().multiplyBy (-1.0);
    }
}


}  // namespace foleys::dsp
