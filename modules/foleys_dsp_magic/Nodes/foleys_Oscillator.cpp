//
// Created by Daniel Walz on 25.02.24.
//

#include "foleys_Oscillator.h"

#include <cmath>

namespace foleys::dsp
{

Oscillator::Oscillator (DspProgram& program, const juce::ValueTree& config) : DspNode (program, config)
{
    addAudioOutput (TRANS ("Audio Out"));
    addParameterOutput (TRANS ("Signal Out"));

    addParameterInput (TRANS ("Signal Type"));
    addParameterInput (TRANS ("Frequency"));
    addParameterInput (TRANS ("Amplitude"));
    addParameterInput (TRANS ("Offset"));
}

void Oscillator::prepare (juce::dsp::ProcessSpec spec)
{
    oscillator.setFrequency (440.0f);
    oscillator.initialise ([] (float t) { return std::sin (t); }, 1024);
    oscillator.prepare (spec);

    buffer.setSize (static_cast<int> (spec.numChannels), static_cast<int> (spec.maximumBlockSize));
}

void Oscillator::process (int numSamples)
{
    buffer.clear();

    auto* signalType = getConnectedOutput (ConnectionType::Parameter, 0);
    auto* frequency  = getConnectedOutput (ConnectionType::Parameter, 1);
    auto* amplitude  = getConnectedOutput (ConnectionType::Parameter, 2);
    auto* dcOffset   = getConnectedOutput (ConnectionType::Parameter, 3);

    if (frequency)
    {
        oscillator.setFrequency (std::max (0.0f, std::lerp (0.0f, 20000.0f, frequency->normalize (frequency->getStaticValue()))));
    }

    auto block = juce::dsp::AudioBlock<float> (buffer).getSubBlock (0, numSamples);
    oscillator.process (juce::dsp::ProcessContextReplacing (block));

    if (amplitude)
        amplitude->multiply (block, 0.0f, 1.0f);

    if (dcOffset)
        dcOffset->add (block, -1.0f, 1.0f);

    auto* audioOutput = getOutput (ConnectionType::Audio, 0);
    audioOutput->setAudioBlock (block);

    auto* parameterOutput = getOutput (ConnectionType::Parameter, 0);
    parameterOutput->setAudioBlock (block);
}

}  // namespace foleys::dsp
