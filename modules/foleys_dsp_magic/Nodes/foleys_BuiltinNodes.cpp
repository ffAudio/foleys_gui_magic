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
        output->setAudioBlock (juce::dsp::AudioBlock<float> (data, static_cast<size_t> (numChannels), static_cast<size_t> (numSamples)));
}

// ================================================================================

AudioOutput::AudioOutput (DspProgram& program, const juce::ValueTree& node) : DspNode (program, node)
{
    addAudioInput (TRANS ("Audio In"));
}

void AudioOutput::setAudioBuffer (float* const* data, int numChannels, int numSamples)
{
    buffer.setDataToReferTo (data, numChannels, numSamples);
}

void AudioOutput::process ([[maybe_unused]] int numSamples)
{
    auto* source = getConnectedOutput (ConnectionType::Audio, 0);
    if (!source)
    {
        buffer.clear();
        return;
    }

    auto audio = source->getAudio();
    for (int ch = 0; ch < std::min (buffer.getNumChannels(), static_cast<int> (audio.getNumChannels())); ++ch)
        if (buffer.getWritePointer (ch) != audio.getChannelPointer (static_cast<size_t> (ch)))
            buffer.copyFrom (ch, 0, source->getAudio().getChannelPointer (static_cast<size_t> (ch)), buffer.getNumSamples());

    for (int ch = static_cast<int> (audio.getNumChannels()); ch < buffer.getNumChannels(); ++ch)
        buffer.clear (ch, 0, buffer.getNumSamples());
}


// ================================================================================

ParameterInput::ParameterInput (DspProgram& program, const juce::ValueTree& node) : DspNode (program, node) { }

void ParameterInput::process ([[maybe_unused]] int numSamples)
{
    for (size_t i = 0; i < parameters.size(); ++i)
    {
        if (auto* parameterOutput = getOutput (ConnectionType::Parameter, static_cast<int> (i)))
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