//
// Created by Daniel Walz on 03.05.24.
//

#pragma once

namespace foleys::dsp
{

class Summing : public DspNode
{
public:
    Summing (DspProgram& program, const juce::ValueTree& config);
    [[nodiscard]] juce::String getCategory() const override { return Category::Audio; }

    void prepare (juce::dsp::ProcessSpec spec) override;
    void process() override;

    FOLEYS_DECLARE_DSP_FACTORY (Summing)
private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Summing)
};


}  // namespace foleys::dsp
