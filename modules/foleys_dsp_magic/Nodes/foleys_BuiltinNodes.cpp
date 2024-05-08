//
// Created by Daniel Walz on 04.02.24.
//

#include "foleys_BuiltinNodes.h"

#include "foleys_dsp_magic/DSP/foleys_MagicDspBuilder.h"

namespace foleys::dsp
{

AudioInput::AudioInput (DspProgram& program, const juce::ValueTree& node) : DspNode (program, node)
{
    addAudioOutput (TRANS ("Audio Out"));
}

void AudioInput::setAudioBuffer (float* const* data, int numChannels, int numSamples)
{
    if (auto* output = getOutput (ConnectionType::Audio, 0))
        output->setAudioBlock (juce::dsp::AudioBlock<float> (data, numChannels, numSamples));
}

// ================================================================================

AudioOutput::AudioOutput (DspProgram& program, const juce::ValueTree& node) : DspNode (program, node)
{
    addAudioInput (TRANS ("Audio In"));
}

// ================================================================================

ParameterInput::ParameterInput (DspProgram& program, const juce::ValueTree& node) : DspNode (program, node) { }

void ParameterInput::process ([[maybe_unused]] int numSamples)
{
    for (size_t i = 0; i < parameters.size(); ++i)
    {
        if (auto* parameterOutput = getOutput (ConnectionType::Parameter, i))
        {
            auto* parameter = parameters[i];
            parameterOutput->setStaticValue (parameter->convertFrom0to1 (parameter->getValue()));
            parameterOutput->setRange (parameter->getNormalisableRange().start, parameter->getNormalisableRange().end);
        }
    }
}

void ParameterInput::setParameterGroup (const juce::AudioProcessorParameterGroup& group)
{
    parameters.clear();
    clearOutputs (ConnectionType::Parameter);

    for (const auto& p: group.getParameters (true))
    {
        if (auto* parameter = dynamic_cast<juce::RangedAudioParameter*> (p))
        {
            addParameterOutput (parameter->getName (255));
            parameters.push_back (parameter);
        }
    }
}

}  // namespace foleys::dsp