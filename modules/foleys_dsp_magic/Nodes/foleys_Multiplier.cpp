//
// Created by Daniel Walz on 22.05.24.
//

#include "foleys_Multiplier.h"


namespace foleys::dsp
{

Multiplier::Multiplier (DspProgram& program, const juce::ValueTree& config) : DspNode (program, config)
{
    addParameterInput (TRANS ("Param In"), 0.0f, 0.0f, 1.0f);
    addParameterInput (TRANS ("Multiplier"), 1.0f, 0.0f, 1.0f);
    addParameterInput (TRANS ("DC Offset"), 0.0f, -1.0f, 1.0f);
    addParameterInput (TRANS ("Phase"), 0.0f, 0.0f, 1.0f);

    addParameterOutput (TRANS ("Param Out"));
}

void Multiplier::prepare ([[maybe_unused]] juce::dsp::ProcessSpec spec) { }

void Multiplier::process ([[maybe_unused]] int numSamples)
{
    auto* output = getOutput (ConnectionType::Parameter, 0);
    jassert (output);

    output->setStaticValue (0.0);

    if (auto* input = getConnectedOutput (ConnectionType::Parameter, 0))
    {
        output->setRange (input->getRangeMin(), input->getRangeMax());

        if (!input->isStatic())
        {
            auto block = input->getAudio();
            output->setAudioBlock (block);
            if (auto* multiplier = getConnectedOutput (ConnectionType::Parameter, 1))
            {
                multiplier->multiply (block);
            }
        }
        else
        {
            if (auto* multiplier = getConnectedOutput (ConnectionType::Parameter, 1))
            {
                if (!multiplier->isStatic())
                {
                    auto block = multiplier->getAudio();
                    output->setAudioBlock (block);
                    block *= input->getStaticValue();
                }
            }
        }
    }

    if (auto* dc = getInput (ConnectionType::Parameter, 2))
    {
        if (output->isStatic())
        {
            auto outputValue = output->getStaticValue();
            if (dc->isStatic())
            {
                output->setStaticValue (outputValue * dc->getStaticValue());
            }
            else
            {
                auto dcBlock = dc->getOutput()->getAudio();
                dcBlock += outputValue;
                output->setAudioBlock (dcBlock);
            }
        }
        else
        {
            auto outputBlock = output->getAudio();
            if (dc->isStatic())
                outputBlock += dc->getStaticValue();
            else
                outputBlock += dc->getOutput()->getAudio();
        }
    }

    if (getInput (ConnectionType::Parameter, 3)->getStaticValue() > 0.5f)
    {
        if (output->isStatic())
            output->setStaticValue (output->getStaticValue() * -1.0f);
        else
        {
            auto block = output->getAudio();
            block *= -1.0f;
        }
    }
}


}  // namespace foleys::dsp
