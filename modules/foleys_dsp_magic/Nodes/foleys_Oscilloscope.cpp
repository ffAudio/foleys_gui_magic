//
// Created by Daniel Walz on 25.02.24.
//

#include "foleys_Oscilloscope.h"

namespace foleys::dsp
{

Oscilloscope::Oscilloscope (DspProgram& program, const juce::ValueTree& config) : DspNode (program, config)
{
    addAudioInput (TRANS ("Audio In"));
    addAudioOutput (TRANS ("Audio Out"));

    addParameterInput (TRANS ("Parameter Input"));

    auto& state = program.getMagicProcessorState();
    scope       = state.getObjectWithType<MagicOscilloscope> (getName());

    if (!scope)
        scope = state.createAndAddObject<MagicOscilloscope> (getName());
}

void Oscilloscope::prepare (juce::dsp::ProcessSpec spec)
{
    if (!scope)
        return;

    scope->prepareToPlay (spec.sampleRate, spec.maximumBlockSize);
}

void Oscilloscope::process ([[maybe_unused]] int numSamples)
{
    auto* audioOutput = getOutput (ConnectionType::Audio, 0);
    jassert (audioOutput);

    audioOutput->setAudioBlock ({});

    if (!scope)
        return;

    auto* source = getConnectedOutput (ConnectionType::Audio, 0);
    if (!source)
        source = getConnectedOutput (ConnectionType::Parameter, 0);

    if (!source)
        return;

    auto audio = source->getAudio();
    if (audio.getNumSamples() == 0)
        return;

    float* pointers[audio.getNumChannels()];
    for (size_t c = 0; c < audio.getNumChannels(); ++c)
        pointers[c] = audio.getChannelPointer (c);

    juce::AudioBuffer<float> proxy (pointers, static_cast<int> (audio.getNumChannels()), static_cast<int> (audio.getNumSamples()));
    scope->pushSamples (proxy);

    audioOutput->setAudioBlock (audio);
}


}  // namespace foleys::dsp
