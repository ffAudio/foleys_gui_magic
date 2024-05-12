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
    addParameterInput (TRANS ("DC"));
    addParameterInput (TRANS ("Phase"));

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
    if (auto* phaseParameter = getConnectedOutput (ConnectionType::Parameter, 2))
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

    if (auto* offset = getConnectedOutput (ConnectionType::Parameter, 1))
        offset->add (audioOutput->getAudio(), -1.0f, 1.0f);
}


}  // namespace foleys::dsp
