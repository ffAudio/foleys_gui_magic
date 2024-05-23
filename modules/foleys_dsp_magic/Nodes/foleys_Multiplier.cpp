//
// Created by Daniel Walz on 22.05.24.
//

#include "foleys_Multiplier.h"


namespace foleys::dsp
{

Multiplier::Multiplier (DspProgram& program, const juce::ValueTree& config) : DspNode (program, config)
{
    addParameterInput (TRANS ("Param In"));
    addParameterInput (TRANS ("Multiplier"));
    addParameterInput (TRANS ("DC Offset"));
    addParameterInput (TRANS ("Phase"));

    addParameterOutput (TRANS ("Param Out"));
}

void Multiplier::prepare ([[maybe_unused]] juce::dsp::ProcessSpec spec) { }

void Multiplier::process ([[maybe_unused]] int numSamples)
{
    auto* output = getOutput (ConnectionType::Parameter, 0);
    jassert (output);

    output->setStaticValue (0.0);

    bool phaseSwap = false;
    if (auto* phaseParameter = getConnectedOutput (ConnectionType::Parameter, 3))
        phaseSwap = phaseParameter->getStaticValue() < 0.5f;

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
                if (phaseSwap)
                    block *= -1.0;
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
                    if (phaseSwap)
                        block *= -1.0;
                }
            }
        }
    }

    if (auto* dc = getConnectedOutput (ConnectionType::Parameter, 2))
    {
        if (output->isStatic())
        {
            auto outputValue = output->getStaticValue();
            if (dc->isStatic())
            {
                if (phaseSwap)
                    output->setStaticValue (outputValue * dc->getStaticValue() * -1.0f);
                else
                    output->setStaticValue (outputValue * dc->getStaticValue());
            }
            else
            {
                auto block = dc->getAudio();
                block += outputValue;
                output->setAudioBlock (block);
                if (phaseSwap)
                    block *= -1.0f;
            }
        }
        else
        {
            dc->add (output->getAudio());
        }
    }
}


}  // namespace foleys::dsp
