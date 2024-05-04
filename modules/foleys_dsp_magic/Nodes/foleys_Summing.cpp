//
// Created by Daniel Walz on 03.05.24.
//

#include "foleys_Summing.h"


namespace foleys::dsp
{

Summing::Summing (DspProgram& program, const juce::ValueTree& config) : DspNode (program, config)
{
    for (int i = 0; i < 8; ++i)
        addAudioInput (TRANS ("Audio ") + juce::String (i + 1));

    addAudioOutput (TRANS ("Audio Out"));
}


void Summing::prepare (juce::dsp::ProcessSpec spec) { }

void Summing::process()
{
    auto* audioOutput = getOutput(ConnectionType::Audio, 0);
    jassert(audioOutput);

    audioOutput->setAudioBlock ({});

    for (const auto& input: getAudioInputs())
    {
        auto* output = input.getOutput();
        if (!output)
            continue;

        if (audioOutput->getAudio().getNumSamples() == 0)
            audioOutput->setAudioBlock (output->getAudio());
        else if (output->getAudio().getNumSamples() > 0)
            audioOutput->getAudio().add (output->getAudio());
    }
}

}  // namespace foleys::dsp
