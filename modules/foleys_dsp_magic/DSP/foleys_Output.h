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

    float getRangeMin() const { return minValue; }
    float getRangeMax() const { return maxValue; }

    void add (const juce::dsp::AudioBlock<float>& target);
    void multiply (const juce::dsp::AudioBlock<float>& target);

    void add (const juce::dsp::AudioBlock<float>& target, float targetMin, float targetMax);
    void multiply (const juce::dsp::AudioBlock<float>& target, float targetMin, float targetMax);

    void setRange (float min, float max);
    void setAudioBlock (juce::dsp::AudioBlock<float> block);

    DspNode& getOwner() { return owner; }

    DspNode&           owner;
    const juce::String name;

    float normalize (float value) const { return (value - minValue) / (maxValue - minValue); }

private:
    bool                         staticFlag  = true;
    float                        staticValue = 0.0f;
    float                        minValue    = -1.0f;
    float                        maxValue    = 1.0f;
    juce::dsp::AudioBlock<float> audio;
};


}  // namespace foleys::dsp
