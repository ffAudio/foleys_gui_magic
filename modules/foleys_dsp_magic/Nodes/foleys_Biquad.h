//
// Created by Daniel Walz on 21.04.24.
//

#pragma once

namespace foleys::dsp
{

class Biquad : public DspNode
{
public:
    Biquad (DspProgram& program, const juce::ValueTree& config);
    [[nodiscard]] juce::String getCategory() const override { return Category::Filters; }

    void prepare (juce::dsp::ProcessSpec spec) override;
    void process (juce::dsp::AudioBlock<float>& buffer, juce::MidiBuffer& midi) override;
    void release() override;

    [[nodiscard]] int getNumAudioInputs() const override { return 1; }
    [[nodiscard]] int getNumAudioOutputs() const override { return 1; }
    [[nodiscard]] int getNumParameterInputs() const override;
    [[nodiscard]] juce::String getParameterInputName (int index) const override;


    FOLEYS_DECLARE_DSP_FACTORY (Biquad)
private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Biquad)
};


}  // namespace foleys::dsp
