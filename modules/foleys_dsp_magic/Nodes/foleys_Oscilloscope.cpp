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

void Oscilloscope::process()
{
    auto* audioOutput = getOutput(ConnectionType::Audio, 0);
    jassert(audioOutput);

    audioOutput->setAudioBlock ({});

    if (!scope)
        return;

    if (auto* output = getConnectedOutput (ConnectionType::Audio, 0))
    {
        auto   audio = output->getAudio();
        float* pointers[audio.getNumChannels()];
        for (size_t c = 0; c < audio.getNumChannels(); ++c)
            pointers[c] = audio.getChannelPointer (c);

        juce::AudioBuffer<float> proxy (pointers, static_cast<int> (audio.getNumChannels()), static_cast<int> (audio.getNumSamples()));
        scope->pushSamples (proxy);

        audioOutput->setAudioBlock (audio);
    }
}


}  // namespace foleys::dsp
