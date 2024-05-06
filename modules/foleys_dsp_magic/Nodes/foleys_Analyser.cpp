//
// Created by Daniel Walz on 25.02.24.
//

#include "foleys_Analyser.h"

#include "../DSP/foleys_Connection.h"
#include "../DSP/foleys_Output.h"

namespace foleys::dsp
{

Analyser::Analyser (DspProgram& program, const juce::ValueTree& config) : DspNode (program, config)
{
    addAudioInput (TRANS ("Audio In"));
    addAudioOutput (TRANS ("Audio Out"));

    auto& state = program.getMagicProcessorState();
    analyser    = state.getObjectWithType<MagicAnalyser> (getName());

    if (!analyser)
        analyser = state.createAndAddObject<MagicAnalyser> (getName());
}

void Analyser::prepare (juce::dsp::ProcessSpec spec)
{
    if (!analyser)
        return;

    analyser->prepareToPlay (spec.sampleRate, spec.maximumBlockSize);
}

void Analyser::process ([[maybe_unused]] int numSamples)
{
    auto* audioOutput = getOutput (ConnectionType::Audio, 0);
    jassert (audioOutput);

    audioOutput->setAudioBlock ({});

    if (!analyser)
        return;

    if (auto* output = getConnectedOutput (ConnectionType::Audio, 0))
    {
        auto   audio = output->getAudio();
        float* pointers[audio.getNumChannels()];
        for (size_t c = 0; c < audio.getNumChannels(); ++c)
            pointers[c] = audio.getChannelPointer (c);

        juce::AudioBuffer<float> proxy (pointers, static_cast<int> (audio.getNumChannels()), static_cast<int> (audio.getNumSamples()));
        analyser->pushSamples (proxy);

        audioOutput->setAudioBlock (audio);
    }
}


}  // namespace foleys::dsp
