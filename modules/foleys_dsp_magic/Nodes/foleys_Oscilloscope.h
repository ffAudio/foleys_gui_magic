//
// Created by Daniel Walz on 25.02.24.
//

#pragma once

#include "../DSP/foleys_DspNode.h"

namespace foleys::dsp
{

class Oscilloscope : public DspNode
{
public:
    Oscilloscope (DspProgram& program, const juce::ValueTree& config);
    [[nodiscard]] juce::String getCategory() const override { return Category::Visualisers; }

    void prepare (juce::dsp::ProcessSpec spec) override;
    void process() override;

    FOLEYS_DECLARE_DSP_FACTORY (Oscilloscope)
private:
    MagicOscilloscope* scope = nullptr;


    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Oscilloscope)
};


}  // namespace foleys::dsp
