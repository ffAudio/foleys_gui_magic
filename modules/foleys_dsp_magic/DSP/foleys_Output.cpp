//
// Created by Daniel Walz on 29.04.24.
//

#include "foleys_Output.h"


namespace foleys::dsp
{

Output::Output (DspNode& o, const juce::String& n) : owner (o), name (n) { }

bool Output::isStatic() const
{
    return staticFlag;
}

float Output::getStaticValue() const
{
    return staticValue;
}

void Output::setStaticValue (float value)
{
    staticValue = value;
    staticFlag  = true;
}

float Output::getValueAt (int samplePos) const
{
    if (staticFlag)
        return staticValue;

    return audio.getSample (0, samplePos);
}

juce::dsp::AudioBlock<float> Output::getAudio()
{
    return audio;
}

void Output::add (const juce::dsp::AudioBlock<float>& target)
{
    if (staticFlag)
        target.add (staticValue);
    else
        target.add (audio);
}

void Output::multiply (const juce::dsp::AudioBlock<float>& target)
{
    if (staticFlag)
        target.multiplyBy (staticValue);
    else
        target.multiplyBy (audio);
}

void Output::setAudioBlock (juce::dsp::AudioBlock<float> block)
{
    audio      = block;
    staticFlag = false;
}


}  // namespace foleys::dsp
