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
    Oscillator (DspProgram& program, const juce::ValueTree& config);
    [[nodiscard]] juce::String getCategory() const override { return Category::Generators; }

    void prepare (juce::dsp::ProcessSpec spec) override;
    void process (juce::dsp::AudioBlock<float>& buffer, juce::MidiBuffer& midi) override;
    void release() override;

    [[nodiscard]] int getNumAudioOutputs() const override { return 1; }
    [[nodiscard]] int getNumParameterInputs() const override;
    [[nodiscard]] juce::String getParameterInputName (int index) const override;

    FOLEYS_DECLARE_DSP_FACTORY (Oscillator)
private:
    juce::dsp::Oscillator<float> m_oscillator;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Oscillator)
};


}  // namespace foleys::dsp
