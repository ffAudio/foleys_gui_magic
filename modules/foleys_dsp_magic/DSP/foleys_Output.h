//
// Created by Daniel Walz on 29.04.24.
//

#pragma once


namespace foleys::dsp
{

struct Output
{
public:
    Output (DspNode& owner, const juce::String& name);

    bool  isStatic() const;
    float getStaticValue() const;
    void  setStaticValue (float value);

    float getValueAt (int samplePos) const;

    juce::dsp::AudioBlock<float> getAudio();

    void add (const juce::dsp::AudioBlock<float>& target);
    void multiply (const juce::dsp::AudioBlock<float>& target);

    void setAudioBlock (juce::dsp::AudioBlock<float> block);

    DspNode& getOwner() { return owner; }

    DspNode&           owner;
    const juce::String name;

private:
    bool                         staticFlag  = true;
    float                        staticValue = 0.0f;
    juce::dsp::AudioBlock<float> audio;
};


}  // namespace foleys::dsp
