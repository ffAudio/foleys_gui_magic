//
// Created by Daniel Walz on 25.02.24.
//

#include "foleys_Oscillator.h"

namespace foleys::dsp
{

Oscillator::Oscillator (DspProgram& program, const juce::ValueTree& config) : DspNode (program, config)
{
    addAudioOutput (TRANS ("Audio Out"));

    addParameterInput (TRANS ("Signal Type"));
    addParameterInput (TRANS ("Frequency"));
    addParameterInput (TRANS ("Gain"));
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

    auto* frequency = getConnectedOutput (ConnectionType::Parameter, 1);
    if (frequency)
        oscillator.setFrequency(frequency->getStaticValue());

//    auto* gain      = getConnectedOutput (ConnectionType::Parameter, 2);


    auto block = juce::dsp::AudioBlock<float> (buffer).getSubBlock (0, numSamples);
    oscillator.process (juce::dsp::ProcessContextReplacing (block));

    auto* output = getOutput (ConnectionType::Audio, 0);
    output->setAudioBlock (block);
}

}  // namespace foleys::dsp
