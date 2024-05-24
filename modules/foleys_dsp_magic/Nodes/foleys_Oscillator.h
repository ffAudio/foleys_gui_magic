//
// Created by Daniel Walz on 25.02.24.
//

#pragma once

#include "../DSP/foleys_DspNode.h"

namespace foleys::dsp
{

class Oscillator : public DspNode
{
public:
    enum WaveType
    {
        None = 0,
        Sine,
        Saw,
        Square
    };


    Oscillator (DspProgram& program, const juce::ValueTree& config);
    [[nodiscard]] juce::String getCategory() const override { return Category::Generators; }

    void prepare (juce::dsp::ProcessSpec spec) override;
    void process (int numSamples) override;

    void setWaveType (WaveType type);

    FOLEYS_DECLARE_DSP_FACTORY (Oscillator)
private:
    juce::AudioBuffer<float> buffer;
    WaveType                 currentWaveType = None;
    std::vector<float>       wavetable;
    double                   sampleRate = 0.0;
    float                    phase      = 0.0f;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Oscillator)
};


}  // namespace foleys::dsp
